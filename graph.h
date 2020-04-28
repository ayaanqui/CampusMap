//
// Ayaan Siddiqui
//
// Basic graph class using adjacency matrix representation.  Currently
// limited to a graph with at most 100 vertices.
//
// original author: Prof. Joe Hummel
// U. of Illinois, Chicago
// CS 251: Spring 2020
//
#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>
#include <map>

template <typename VertexT, typename WeightT>
class graph
{
private:
    struct VertexData
    {
        std::map<VertexT, WeightT> neighbors;
        std::set<VertexT> neighborsV;
    };

    std::map<VertexT, VertexData> vertices;
    int numEdges;

public:
    graph()
    {
        this->numEdges = 0;
    }

    graph(const graph &other)
    {
        this->vertices = other.vertices;
        this->numEdges = other.numEdges;
    }

    graph &operator=(const graph &other)
    {
        this->vertices = other.vertices;
        this->numEdges = other.numEdges;

        return *this;
    }

    int NumVertices() const
    {
        return this->vertices.size();
    }

    int NumEdges() const
    {
        return this->numEdges;
    }

    bool addVertex(VertexT v)
    {
        if (this->vertices.count(v) > 0)
            return false;

        this->vertices.emplace(v, VertexData{});
        return true;
    }

    bool addEdge(VertexT from, VertexT to, WeightT weight)
    {
        auto it = this->vertices.find(from);
        if (it == this->vertices.end())
            return false; // Vertex not found, so return false

        it->second.neighbors.emplace(to, weight);
        it->second.neighborsV.emplace(to);

        this->numEdges++;

        return true;
    }

    bool getWeight(VertexT from, VertexT to, WeightT &weight)
    {
        auto it = this->vertices.find(from);
        if (it == this->vertices.end())
            return false;

        auto itInner = it->second.neighbors.find(to);
        if (itInner == it->second.neighbors.end())
            return false;

        weight = itInner->second;
        return true;
    }

    std::set<VertexT> neighbors(VertexT v) const
    {
        return this->vertices.find(v)->second.neighborsV;
    }

    std::vector<VertexT> getVertices()
    {
        std::vector<VertexT> v;
        for (auto i : this->vertices)
            v.push_back(i.first);
        return v; // returns a copy:
    }

    //
    // dump
    //
    // Dumps the internal state of the graph for debugging purposes.
    //
    // Example:
    //    Graph<string,int>  G(26);
    //    ...
    //    G.dump(std::cout);  // dump to console
    //
    void dump(std::ostream &output) const
    {
        output << "***************************************************" << std::endl;
        output << "********************* GRAPH ***********************" << std::endl;

        output << "**Num vertices: " << this->NumVertices() << std::endl;
        output << "**Num edges: " << this->NumEdges() << std::endl;

        output << std::endl;
        output << "**Vertices:" << std::endl;
        auto it = this->vertices.begin();

        for (int i = 0; i < this->NumVertices(); ++i)
        {
            output << " " << i << ". " << it->first << std::endl;
            it++;
        }

        output << std::endl;
        output << "**Edges:" << std::endl;
        it = this->vertices.begin();

        for (int row = 0; row < this->NumVertices(); ++row)
        {
            output << " row " << row << ": ";
            auto itInner = this->vertices.begin();

            for (int col = 0; col < this->NumVertices(); ++col)
            {
                auto elem = it->second.neighbors.find(itInner->first);
                if (elem == it->second.neighbors.end())
                    output << "F ";
                else
                    output << "(T," << elem->second << ") ";
                itInner++;
            }
            it++;
            output << std::endl;
        }

        output << "**************************************************" << std::endl;
    }
};