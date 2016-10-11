#include "OctreeNode.h"
#include "Vertex.h"
#include "World.h"
#include "Tables.h"
#include <set>

namespace MDC
{
	OctreeNode::OctreeNode() : position(XMINT3(0, 0, 0)), size(1), is_leaf(true), vertices(0), corners(0), child_index(0), vertex_count(0), scale(1)
	{
		for (int i = 0; i < 8; i++)
			children[i] = 0;
	}

	OctreeNode::OctreeNode(XMINT3 position, unsigned short size, bool is_leaf, unsigned int scale) : position(position), size(size), is_leaf(is_leaf), vertices(0), corners(0), child_index(0), vertex_count(0), scale(scale)
	{
		for (int i = 0; i < 8; i++)
			children[i] = 0;
	}

	OctreeNode::~OctreeNode()
	{
		for (int i = 0; i < 8; i++)
		{
			if (children[i])
			{
				delete children[i];
				children[i] = 0;
			}
		}
		if (vertices)
		{
			delete[] vertices;
			vertices = 0;
		}
	}

	bool OctreeNode::ConstructNodes()
	{
		if (size == 1)
			return ConstructLeaf();

		is_leaf = false;
		unsigned short child_size = size / 2;
		bool has_children = false;

		OctreeNode temp_node;
		for (int i = 0; i < 8; i++)
		{
			XMINT3 child_pos = XMINT3(position.x + i / 4 * child_size * scale, position.y + i % 4 / 2 * child_size * scale, position.z + i % 2 * child_size * scale);
			temp_node = OctreeNode(child_pos, child_size, false, scale);
			temp_node.child_index = i;
			if (!temp_node.ConstructNodes())
			{
				children[i] = 0;
				continue;
			}

			has_children = true;
			children[i] = new OctreeNode(child_pos, child_size, false, scale);
			memcpy(children[i], &temp_node, sizeof(temp_node));

		}

		ZeroMemory(&temp_node, sizeof(temp_node));
		if (!has_children)
			is_leaf = true;

		return has_children;
	}

	bool OctreeNode::ConstructLeaf()
	{
		if (size != 1)
			return false;

		is_leaf = true;
		corners = 0;
		float samples[8];
		for (int i = 0; i < 8; i++)
		{
			if ((samples[i] = World::Sample(position.x + i / 4 * scale, position.y + i % 4 / 2 * scale, position.z + i % 2 * scale, scale)) < 0)
				corners |= 1 << i;
		}

		if (corners == 0 || corners == 255)
		{
			vertex_count = 0;
			return false;
		}

		int v_edges[4][12]; //the edges corresponding to each vertex

		int v_index = 0;
		int e_index = 0;
		Vector3 vpos((float)position.x, (float)position.y, (float)position.z);

		for (int e = 0; e < 16; e++)
		{
			int code = edge_table[corners][e];
			if (code == -2)
			{
				v_edges[v_index++][e_index] = -1;
				break;
			}
			if (code == -1)
			{
				v_edges[v_index++][e_index] = -1;
				e_index = 0;
				continue;
			}

			v_edges[v_index][e_index++] = code;
		}

		vertices = new Vertex[v_index];
		vertex_count = v_index;

		for (int i = 0; i < v_index; i++)
		{
			int k = 0;
			Vec3 pos;
			Vector3 mpos;
			Vector3 normal(0, 0, 0);
			QEFSolver qef;
			int ei[12] = { 0 };
			while (v_edges[i][k] != -1)
			{
				ei[v_edges[i][k]] = 1;
				Vector3 a = vpos + corner_deltas_f[edge_pairs[v_edges[i][k]][0]] * (float)size * (float)scale;
				Vector3 b = vpos + corner_deltas_f[edge_pairs[v_edges[i][k]][1]] * (float)size * (float)scale;
				//Vector3 intersection = World::GetIntersection(a, b, samples[edge_pairs[v_edges[i][k]][0]], samples[edge_pairs[v_edges[i][k]][1]]);
				Vector3 intersection = World::GetIntersection(a, b, 2);

				Vector3 n = World::GetNormal(intersection, scale);
				normal += n;
				mpos += intersection;
				qef.add(intersection.x, intersection.y, intersection.z, n.x, n.y, n.z);
				k++;
			}

			mpos /= (float)k;
			normal /= (float)k;
			normal.Normalize();
			/* If this is enabled, the normal quality is much lower */
			//normal = World::GetNormal(pos, scale);

			this->vertices[i].index = -1; //this gets set during the vertex buffer generation
			this->vertices[i].parent = 0;
			this->vertices[i].error = 0;// = qef.solve(pos, 1e-6f, 4, 1e-6f);
			//this->vertices[i].position = Vector3(pos.x, pos.y, pos.z);
			this->vertices[i].normal = normal;
			this->vertices[i].euler = 1;
			this->vertices[i].in_cell = this->child_index;
			this->vertices[i].flags |= VertexFlags::COLLAPSIBLE | VertexFlags::FACEPROP2;

			memcpy(this->vertices[i].eis, ei, sizeof(int) * 12);
			memcpy(&this->vertices[i].qef, &qef, sizeof(qef));
		}

		return true;
	}

	void OctreeNode::GenerateVertexBuffer(std::vector<VertexPositionColorNormal>& v_out)
	{
		if (!is_leaf)
		{
			for (int i = 0; i < 8; i++)
			{
				if (children[i])
					children[i]->GenerateVertexBuffer(v_out);
			}
		}

		if (!vertices || vertex_count == 0)
			return;

		for (int i = 0; i < vertex_count; i++)
		{
			vertices[i].index = v_out.size();
			VertexPositionColorNormal v;
			Vec3 p;
			vertices[i].qef.solve(p, 1e-6, 4, 1e-6);
			v.position = Vector3(p.x, p.y, p.z);
			v.normal = vertices[i].normal;
			v.color = World::GetColor(v.position);
			v_out.push_back(v);
		}
	}

	void OctreeNode::ProcessCell(std::vector<unsigned int>& indexes, float threshold)
	{
		if (is_leaf)
			return;

		for (int i = 0; i < 8; i++)
		{
			if (children[i])
				children[i]->ProcessCell(indexes, threshold);
		}

		OctreeNode* face_nodes[2];
		OctreeNode* edge_nodes[4];
		for (int i = 0; i < 12; i++)
		{
			face_nodes[0] = children[edge_pairs[i][0]];
			face_nodes[1] = children[edge_pairs[i][1]];

			ProcessFace(face_nodes, edge_pairs[i][2], indexes, threshold);
		}

		for (int i = 0; i < 6; i++)
		{
			edge_nodes[0] = children[cell_proc_edge_mask[i][0]];
			edge_nodes[1] = children[cell_proc_edge_mask[i][1]];
			edge_nodes[2] = children[cell_proc_edge_mask[i][2]];
			edge_nodes[3] = children[cell_proc_edge_mask[i][3]];

			ProcessEdge(edge_nodes, cell_proc_edge_mask[i][4], indexes, threshold);
		}
	}

	void OctreeNode::ProcessFace(OctreeNode ** nodes, int direction, std::vector<unsigned int>& indexes, float threshold)
	{
		if (!nodes[0] || !nodes[1])
			return;

		if (!nodes[0]->is_leaf || !nodes[1]->is_leaf)
		{
			OctreeNode* face_nodes[2];
			OctreeNode* edge_nodes[4];
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					if (nodes[j]->is_leaf)
						face_nodes[j] = nodes[j];
					else
						face_nodes[j] = nodes[j]->children[face_proc_face_mask[direction][i][j]];
				}

				ProcessFace(face_nodes, face_proc_face_mask[direction][i][2], indexes, threshold);
			}

			const int orders[2][4] =
			{
				{ 0, 0, 1, 1 },
				{ 0, 1, 0, 1 },
			};

			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					if (nodes[orders[face_proc_edge_mask[direction][i][0]][j]]->is_leaf)
						edge_nodes[j] = nodes[orders[face_proc_edge_mask[direction][i][0]][j]];
					else
						edge_nodes[j] = nodes[orders[face_proc_edge_mask[direction][i][0]][j]]->children[face_proc_edge_mask[direction][i][1 + j]];
				}

				ProcessEdge(edge_nodes, face_proc_edge_mask[direction][i][5], indexes, threshold);
			}
		}
	}

	void OctreeNode::ProcessEdge(OctreeNode ** nodes, int direction, std::vector<unsigned int>& indexes, float threshold)
	{
		if (!nodes[0] || !nodes[1] || !nodes[2] || !nodes[3])
			return;

		if (nodes[0]->is_leaf && nodes[1]->is_leaf && nodes[2]->is_leaf && nodes[3]->is_leaf)
			ProcessIndexes(nodes, direction, indexes, threshold);
		else
		{
			OctreeNode* edge_nodes[4];
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					if (nodes[j]->is_leaf)
						edge_nodes[j] = nodes[j];
					else
						edge_nodes[j] = nodes[j]->children[edge_proc_edge_mask[direction][i][j]];
				}

				ProcessEdge(edge_nodes, edge_proc_edge_mask[direction][i][4], indexes, threshold);
			}
		}
	}

	void OctreeNode::ProcessIndexes(OctreeNode ** nodes, int direction, std::vector<unsigned int>& indexes, float threshold)
	{
		unsigned short min_size = 65535;
		unsigned int indices[4] = { -1,-1,-1,-1 };
		bool flip = false;
		bool sign_changed = false;

		for (int i = 0; i < 4; i++)
		{
			int edge = process_edge_mask[direction][i];
			int c1 = edge_pairs[edge][0];
			int c2 = edge_pairs[edge][1];

			int m1 = (nodes[i]->corners >> c1) & 1;
			int m2 = (nodes[i]->corners >> c2) & 1;

			if (nodes[i]->size < min_size)
			{
				min_size = nodes[i]->size;
				flip = m2 == 1;
				sign_changed = ((!m1 && m2) || (m1 && !m2));
			}

			int index = 0;
			bool skip = false;
			for (int k = 0; k < 16; k++)
			{
				int e = edge_table[nodes[i]->corners][k];
				if (e == -1)
				{
					index++;
					continue;
				}
				if (e == -2)
				{
					skip = true;
					break;
				}
				if (e == edge)
					break;
			}

			if (skip)
				continue;
			if (index >= nodes[i]->vertex_count)
				return;

			Vertex* v = &nodes[i]->vertices[index];
			Vertex* highest = v;
			while (highest->parent)
			{
				if ((highest->parent->error <= threshold && highest->parent->IsManifold()))
				{
					highest = highest->parent;
					v = highest;
				}
				else
					highest = highest->parent;
			}
			indices[i] = v->index;
		}

		if (sign_changed)
		{
			if (!flip)
			{
				if (indices[0] != -1 && indices[1] != -1 && indices[2] != -1 && indices[0] != indices[1] && indices[1] != indices[3])
				{
					indexes.push_back(indices[0]);
					indexes.push_back(indices[1]);
					indexes.push_back(indices[3]);
				}

				if (indices[0] != -1 && indices[2] != -1 && indices[3] != -1 && indices[0] != indices[2] && indices[2] != indices[3])
				{
					indexes.push_back(indices[0]);
					indexes.push_back(indices[3]);
					indexes.push_back(indices[2]);
				}
			}
			else
			{
				if (indices[0] != -1 && indices[3] != -1 && indices[1] != -1 && indices[0] != indices[1] && indices[1] != indices[3])
				{
					indexes.push_back(indices[0]);
					indexes.push_back(indices[3]);
					indexes.push_back(indices[1]);
				}

				if (indices[0] != -1 && indices[2] != -1 && indices[3] != -1 && indices[0] != indices[2] && indices[2] != indices[3])
				{
					indexes.push_back(indices[0]);
					indexes.push_back(indices[2]);
					indexes.push_back(indices[3]);
				}
			}
		}
	}

	void OctreeNode::ClusterCellBase(float error)
	{
		if (is_leaf)
			return;

		for (int i = 0; i < 8; i++)
		{
			if (!children[i])
				continue;
			children[i]->ClusterCell(error);
		}
	}

	void OctreeNode::ClusterCell(float error)
	{
		if (is_leaf)
			return;

		for (int i = 0; i < 8; i++)
		{
			if (!children[i] || children[i]->is_leaf)
				continue;
			children[i]->ClusterCell(error);
		}

		int surface_index = 0;
		std::vector<Vertex*> collected_vertices;
		std::vector<Vertex> new_vertices;

		/*
		* Find all the surfaces inside the children that cross the 6 Euclidean edges and the vertices that connect to them
		*/
		OctreeNode* face_nodes[2];
		OctreeNode* edge_nodes[4];
		for (int i = 0; i < 12; i++)
		{
			int c1 = edge_pairs[i][0];
			int c2 = edge_pairs[i][1];

			face_nodes[0] = children[c1];
			face_nodes[1] = children[c2];

			ClusterFace(face_nodes, edge_pairs[i][2], surface_index, collected_vertices);
		}


		for (int i = 0; i < 6; i++)
		{
			edge_nodes[0] = children[cell_proc_edge_mask[i][0]];
			edge_nodes[1] = children[cell_proc_edge_mask[i][1]];
			edge_nodes[2] = children[cell_proc_edge_mask[i][2]];
			edge_nodes[3] = children[cell_proc_edge_mask[i][3]];

			ClusterEdge(edge_nodes, cell_proc_edge_mask[i][4], surface_index, collected_vertices);
		}

		int highest_index = surface_index;
		if (highest_index == -1)
			highest_index = 0;

		for (int i = 0; i < 8; i++)
		{
			if (!children[i])
				continue;
			for (int k = 0; k < children[i]->vertex_count; k++)
			{
				Vertex* v = &children[i]->vertices[k];
				if (!v)
					continue;
				if (v->surface_index == -1)
				{
					v->surface_index = highest_index++;
					collected_vertices.push_back(v);
				}
			}
		}

		if (collected_vertices.size() > 0)
		{
			std::set<int> surface_set;
			for (auto& v : collected_vertices)
			{
				surface_set.insert(v->surface_index);
			}
			if (surface_set.size() == 0)
				return;

			this->vertices = new Vertex[surface_set.size()];
			this->vertex_count = surface_set.size();

			for (int i = 0; i <= highest_index; i++)
			{
				QEFSolver qef;
				Vector3 normal(0, 0, 0);
				Vector3 positions(0, 0, 0);
				int count = 0;
				int edges[12] = { 0 };
				int euler = 0;
				int e = 0;

				/* manifold criterion */
				for (auto& v : collected_vertices)
				{
					if (v->surface_index == i)
					{
						for (int k = 0; k < 3; k++)
						{
							int edge = external_edges[v->in_cell][k];
							edges[edge] += v->eis[edge];
						}
						for (int k = 0; k < 9; k++)
						{
							int edge = internal_edges[v->in_cell][k];
							e += v->eis[edge];
						}

						euler += v->euler;
						qef.add(v->qef.getData());
						normal += v->normal;
						positions += v->position;
						count++;
					}
				}

				if (count == 0)
					continue;

				bool face_prop2 = true;
				for (int f = 0; f < 6 && face_prop2; f++)
				{
					int intersections = 0;
					for (int ei = 0; ei < 4; ei++)
					{
						intersections += edges[faces[f][ei]];
					}
					if (!(intersections == 0 || intersections == 2))
						face_prop2 = false;
				}

				Vertex new_vertex;
				positions /= (float)count;
				normal /= (float)count;
				normal.Normalize();
				//new_vertex.normal = normal;
				new_vertex.normal = World::GetNormal(positions, 1);
				new_vertex.euler = euler - e / 4;
				new_vertex.in_cell = this->child_index;
				memcpy(&new_vertex.qef, &qef, sizeof(qef));
				memcpy(&new_vertex.eis, &edges, sizeof(edges));

				Vec3 p_out;

				qef.solve(p_out, 1e-6f, 4, 1e-6f);
				new_vertex.error = qef.getError();
				if (new_vertex.error <= error)
					new_vertex.flags |= VertexFlags::COLLAPSIBLE;
				if (face_prop2)
					new_vertex.flags |= VertexFlags::FACEPROP2;
				new_vertex.flags |= 8;

				new_vertices.push_back(new_vertex);

				for (auto& v : collected_vertices)
				{
					if (v->surface_index == i)
					{
						if (v != &new_vertex)
							v->parent = &this->vertices[new_vertices.size() - 1];
						else
							v->parent = 0;
					}
				}
			}
		}
		else
			return;

		for (auto& v : collected_vertices)
		{
			v->surface_index = -1;
		}

		memcpy(this->vertices, &new_vertices[0], sizeof(Vertex) * new_vertices.size());
	}

	void OctreeNode::ClusterFace(OctreeNode ** nodes, int direction, int & surface_index, std::vector<Vertex*>& collected_vertices)
	{
		if (!nodes[0] || !nodes[1])
			return;

		if (!nodes[0]->is_leaf || !nodes[1]->is_leaf)
		{
			for (int i = 0; i < 4; i++)
			{
				OctreeNode* face_nodes[2] = { 0, 0 };
				for (int j = 0; j < 2; j++)
				{
					if (!nodes[j])
						continue;
					if (nodes[j]->is_leaf)
						face_nodes[j] = nodes[j];
					else
						face_nodes[j] = nodes[j]->children[face_proc_face_mask[direction][i][j]];
				}

				ClusterFace(face_nodes, face_proc_face_mask[direction][i][2], surface_index, collected_vertices);
			}
		}

		const int orders[2][4] =
		{
			{ 0, 0, 1, 1 },
			{ 0, 1, 0, 1 },
		};

		for (int i = 0; i < 4; i++)
		{
			OctreeNode* edge_nodes[4] = { 0, 0, 0, 0 };
			for (int j = 0; j < 4; j++)
			{
				if (!nodes[orders[face_proc_edge_mask[direction][i][0]][j]])
					continue;
				if (nodes[orders[face_proc_edge_mask[direction][i][0]][j]]->is_leaf)
					edge_nodes[j] = nodes[orders[face_proc_edge_mask[direction][i][0]][j]];
				else
					edge_nodes[j] = nodes[orders[face_proc_edge_mask[direction][i][0]][j]]->children[face_proc_edge_mask[direction][i][1 + j]];
			}

			ClusterEdge(edge_nodes, face_proc_edge_mask[direction][i][5], surface_index, collected_vertices);
		}
	}

	void OctreeNode::ClusterEdge(OctreeNode ** nodes, int direction, int & surface_index, std::vector<Vertex*>& collected_vertices)
	{
		if ((!nodes[0] || nodes[0]->is_leaf) && (!nodes[1] || nodes[1]->is_leaf) && (!nodes[2] || nodes[2]->is_leaf) && (!nodes[3] || nodes[3]->is_leaf))
			ClusterIndexes(nodes, direction, surface_index, collected_vertices);
		else
		{
			for (int i = 0; i < 2; i++)
			{
				OctreeNode* edge_nodes[4] = { 0, 0, 0, 0 };
				for (int j = 0; j < 4; j++)
				{
					if (!nodes[j])
						continue;
					if (nodes[j]->is_leaf)
						edge_nodes[j] = nodes[j];
					else
						edge_nodes[j] = nodes[j]->children[edge_proc_edge_mask[direction][i][j]];
				}

				ClusterEdge(edge_nodes, edge_proc_edge_mask[direction][i][4], surface_index, collected_vertices);
			}
		}
	}

	void OctreeNode::ClusterIndexes(OctreeNode ** nodes, int direction, int & max_surface_index, std::vector<Vertex*>& collected_vertices)
	{
		if (!nodes[0] && !nodes[1] && !nodes[2] && !nodes[3])
			return;

		Vertex* vertices[4] = { 0, 0, 0, 0 };
		int v_count = 0;
		int node_count = 0;

		for (int i = 0; i < 4; i++)
		{
			if (!nodes[i])
				continue;

			int corners = nodes[i]->corners;
			int edge = process_edge_mask[direction][i];
			int c1 = edge_pairs[edge][0];
			int c2 = edge_pairs[edge][1];

			int m1 = (corners >> c1) & 1;
			int m2 = (corners >> c2) & 1;

			int index = 0;
			bool skip = false;
			for (int k = 0; k < 16; k++)
			{
				int e = edge_table[corners][k];
				if (e == -1)
				{
					index++;
					continue;
				}
				if (e == -2)
				{
					if (!((m1 == 0 && m2 != 0) || (m1 != 0 && m2 == 0)))
						skip = true;
					break;
				}
				if (e == edge)
					break;
			}

			if (!skip && index < nodes[i]->vertex_count)
			{
				vertices[i] = &nodes[i]->vertices[index];
				while (vertices[i]->parent)
					vertices[i] = vertices[i]->parent;
				v_count++;
			}
		}

		if (!v_count)
			return;

		int surface_index = -1;

		for (int i = 0; i < 4; i++)
		{
			Vertex* v = vertices[i];
			if (!v)
				continue;
			if (v->surface_index != -1)
			{
				if (surface_index != -1 && surface_index != v->surface_index)
					AssignSurface(collected_vertices, v->surface_index, surface_index);
				else if (surface_index == -1)
					surface_index = v->surface_index;
			}
		}

		if (surface_index == -1)
			surface_index = max_surface_index++;
		for (int i = 0; i < 4; i++)
		{
			Vertex* v = vertices[i];
			if (!v)
				continue;
			if (v->surface_index == -1)
				collected_vertices.push_back(v);
			v->surface_index = surface_index;
		}
	}

	void OctreeNode::AssignSurface(std::vector<Vertex*>& vertices, int from, int to)
	{
		for (auto& v : vertices)
		{
			if (v && v->surface_index == from)
				v->surface_index = to;
		}
	}


}
