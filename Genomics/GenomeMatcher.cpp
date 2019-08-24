//
//  GenomeMatcher.cpp
//  Project4
//
//  Created by Ayesha Kumbhare on 3/10/19.
//  Copyright © 2019 Ayesha Kumbhare. All rights reserved.
//

#include "provided.h"
#include "Trie.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <unordered_map>
//using namespace std;

class GenomeMatcherImpl
{
public:
    GenomeMatcherImpl(int minSearchLength);
    ~GenomeMatcherImpl();
    void addGenome(const Genome& genome);
    int minimumSearchLength() const;
    bool findGenomesWithThisDNA(const std::string& fragment, int minimumLength, bool exactMatchOnly, std::vector<DNAMatch>& matches) const;
    bool findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, std::vector<GenomeMatch>& results) const;
private:
    
    struct genHolder
    {
        std::string genomeName;
        int indexVec;
        int genomePos;
    };
    
    int m_minSearchLength;
    Trie<genHolder> m_trie;
    std::vector<Genome> m_genomesVec;

};

GenomeMatcherImpl::GenomeMatcherImpl(int minSearchLength)
{
    m_minSearchLength = minSearchLength;
}

GenomeMatcherImpl::~GenomeMatcherImpl()
{
    
}

void GenomeMatcherImpl::addGenome(const Genome& genome)
{
    m_genomesVec.push_back(genome); // entire genome(not just subGenome) into private vector
    std::string subGenome = "";
    for (int i = 0; i + minimumSearchLength() <= genome.length(); i++) // as long as the subGenome fits in the genome and doesn't go over
    {
        bool extractSuccess = genome.extract(i, minimumSearchLength(), subGenome); // get the subGenome
        if(extractSuccess)
        {
            genHolder currHolder; // create a holder
            currHolder.genomeName = genome.name(); // update the holder with the appropriate data
            currHolder.genomePos = i;
            currHolder.indexVec = (int) m_genomesVec.size() - 1;
            m_trie.insert(subGenome, currHolder);// subGenome is the key because it's unique and currHolder is the ValueType in this case
        }
    }
}

int GenomeMatcherImpl::minimumSearchLength() const
{
    return m_minSearchLength;
}

bool GenomeMatcherImpl::findGenomesWithThisDNA(const std::string& fragment, int minimumLength, bool exactMatchOnly, std::vector<DNAMatch>& matches) const
{
    if (fragment.size() < minimumLength || minimumLength < minimumSearchLength())
    {
        return false;
    }
    
    bool matchFound = false;
    
    const std::vector<genHolder> find = m_trie.find(fragment.substr(0, minimumSearchLength()), exactMatchOnly); // getting all subfrags (or SNiPs) of fragment that are at least the minimum length and putting it in a vector
    if (!find.empty()) // if there's something of at least the minimum search length
    {
        for (int i = (int) fragment.size(); i >= minimumLength; i--) // for (int i = minimumLength; i <= fragment.size(); i++)
        {
            for (int j = 0; j < find.size(); j++) // for each subfrag we found
            {
                const Genome& g = m_genomesVec[find[j].indexVec]; // we're finding the genome that corresponds to the curr sub
                std::string candidate = "";
                g.extract(find[j].genomePos, i, candidate); // candidate is now the potential match
                
                int length = 0;
                bool addMatch = true;
                bool oneMismatch;
                
                if (exactMatchOnly)
                {
                    oneMismatch = true; // we've already "found a mismatch" and won't allow for finding another one
                }
                else
                {
                    oneMismatch = false;
                }
                
                for (int k = 0; k < candidate.size(); k++)
                {
                    if (candidate[k] == fragment[k])
                    {
                        length++;
                    }
                    else if (candidate[k] != fragment[k] && !oneMismatch)
                    {
                        oneMismatch = true;
                        length++;
                    }
                    else
                    {
                        addMatch = false;
                        break;
                    }
                }
                
                for (int m = 0; m < matches.size(); m++)
                {
                    if (matches[m].genomeName == find[j].genomeName)
                    {
                        addMatch = false;
                    }
                }

                if (addMatch && length >= minimumLength)
                {
                    matchFound = true;
                    DNAMatch match;
                    match.length = length;
                    match.genomeName = find[j].genomeName;
                    match.position = find[j].genomePos;
                    matches.push_back(match);
                }
            }
        }
    }
    
    if (!matchFound)
        return false;
    return true;
}

bool GenomeMatcherImpl::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, std::vector<GenomeMatch>& results) const
{
    if (fragmentMatchLength < minimumSearchLength())
    {
        return false;
    }
    
    int loopCount = 1;
    bool matchFound = false;
    std::unordered_map<std::string, int> tempMap; // int to store the genome length
    for (int i = 0; i < query.length(); i = loopCount * fragmentMatchLength)
    {
        std::vector<DNAMatch> seqMatches;
        std::string sequence = "";
        query.extract(i, fragmentMatchLength, sequence);
        if (findGenomesWithThisDNA(sequence, (int) sequence.size(), exactMatchOnly, seqMatches))
        {
            for (int j = 0; j < seqMatches.size(); j++)
            {
               if (tempMap.find(seqMatches[j].genomeName) == tempMap.end()) // if this name doesnt exist in the temp map yet
               {
                    tempMap.insert(std::unordered_map<std::string, int>::value_type(seqMatches[j].genomeName, 1)); // insert into the map
                }
               else // does exist
               {
                   tempMap.find(seqMatches[j].genomeName)++;
               }
            }
            
            for (std::unordered_map<std::string, int>::const_iterator p = tempMap.begin(); p != tempMap.end(); p++)
            {
                double percent = ((p->second) / (query.length()/fragmentMatchLength)) * 100;
                if (percent >= matchPercentThreshold) // add to our vector
                {
                    matchFound = true;
                    GenomeMatch match;
                    match.genomeName = p->first;
                    match.percentMatch = percent;
                    results.push_back(match);
                    
                }
            }
        }
        loopCount++;
    }
    
    if (!matchFound)
        return false;
    return true;
}

//******************** GenomeMatcher functions ********************************

// These functions simply delegate to GenomeMatcherImpl's functions.
// You probably don't want to change any of this code.

GenomeMatcher::GenomeMatcher(int minSearchLength)
{
    m_impl = new GenomeMatcherImpl(minSearchLength);
}

GenomeMatcher::~GenomeMatcher()
{
    delete m_impl;
}

void GenomeMatcher::addGenome(const Genome& genome)
{
    m_impl->addGenome(genome);
}

int GenomeMatcher::minimumSearchLength() const
{
    return m_impl->minimumSearchLength();
}

bool GenomeMatcher::findGenomesWithThisDNA(const std::string& fragment, int minimumLength, bool exactMatchOnly, std::vector<DNAMatch>& matches) const
{
    return m_impl->findGenomesWithThisDNA(fragment, minimumLength, exactMatchOnly, matches);
}

bool GenomeMatcher::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, std::vector<GenomeMatch>& results) const
{
    return m_impl->findRelatedGenomes(query, fragmentMatchLength, exactMatchOnly, matchPercentThreshold, results);
}
