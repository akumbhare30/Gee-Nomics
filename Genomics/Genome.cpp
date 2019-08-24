//
//  Genome.cpp
//  Project4
//
//  Created by Ayesha Kumbhare on 3/10/19.
//  Copyright © 2019 Ayesha Kumbhare. All rights reserved.
//
#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
//using namespace std;

class GenomeImpl
{
public:
    GenomeImpl(const std::string& nm, const std::string& sequence);
    static bool load(std::istream& genomeSource, std::vector<Genome>& genomes);
    int length() const;
    std::string name() const;
    bool extract(int position, int length, std::string& fragment) const;
private:
    std::string m_name;
    std::string m_sequence;
    
};

GenomeImpl::GenomeImpl(const std::string& nm, const std::string& sequence) // constructor
{
    m_name = nm;
    m_sequence = sequence;
}

bool GenomeImpl::load(std::istream& genomeSource, std::vector<Genome>& genomes)
{
    // .eof, .get
    // .get gets a char
    // .eof checks if we are at the end of a txt file
    // getline gets the entire line until the newline char (doesn't get the newline char)
    std::string sequence = "";
    std::string name = "";
    int charsPerLine = 0;

    //char curr = genomeSource.get();;
    char curr;
    while (genomeSource.get(curr))
    {
        if (charsPerLine > 80) // takes care of the case where there are more than 80 chars per line (in the sequence portion)
        {
            return false;
        }
        
        if (curr == '>')
        {
            if (sequence.empty() && !name.empty()) // if no base lines after name
            {
                return false;
            }
            if (!sequence.empty() && !name.empty())
            {
                genomes.push_back(Genome(name, sequence));
                sequence = ""; // reset
            }
            getline(genomeSource, name);// if it's the first one/ new name
            if (name.empty()) // if name line is empty
            {
                return false;
            }
        }
        
        else if (curr == 'A' || curr == 'T' || curr == 'C' || curr == 'G' || curr == 'N' || curr == 'a' || curr == 't' || curr == 'c' || curr == 'g' || curr == 'n')
        {
            sequence += toupper(curr);
            charsPerLine++;
        }
        
        else if (curr == '\n')
        {
            if (charsPerLine == 0)
            {
                return false;
            }
            charsPerLine = 0; // reseting
        }
        
        else
        {
            return false;
        }
        
      //  curr = genomeSource.get();
    }
    
    if (sequence.empty())
    {
        return false;
    }
    
    genomes.push_back(Genome(name, sequence));
    return true;
}

int GenomeImpl::length() const
{
    return (int) m_sequence.size();
}

std::string GenomeImpl::name() const
{
    return m_name;
}

bool GenomeImpl::extract(int position, int length, std::string& fragment) const
{
    if (length > m_sequence.size() || position < 0 || position + length > m_sequence.size())
    {
        return false;
    }
    else
    {
        fragment = m_sequence.substr(position, length);
        return true;
    }
}

//******************** Genome functions ************************************

// These functions simply delegate to GenomeImpl's functions.
// You probably don't want to change any of this code.

Genome::Genome(const std::string& nm, const std::string& sequence)
{
    m_impl = new GenomeImpl(nm, sequence);
}

Genome::~Genome()
{
    delete m_impl;
}

Genome::Genome(const Genome& other)
{
    m_impl = new GenomeImpl(*other.m_impl);
}

Genome& Genome::operator=(const Genome& rhs)
{
    GenomeImpl* newImpl = new GenomeImpl(*rhs.m_impl);
    delete m_impl;
    m_impl = newImpl;
    return *this;
}

bool Genome::load(std::istream& genomeSource, std::vector<Genome>& genomes)
{
    return GenomeImpl::load(genomeSource, genomes);
}

int Genome::length() const
{
    return m_impl->length();
}

std::string Genome::name() const
{
    return m_impl->name();
}

bool Genome::extract(int position, int length, std::string& fragment) const
{
    return m_impl->extract(position, length, fragment);
}
