
/*
 * refseq.h for MSIsensor
 * Copyright (c) 2013 Beifang Niu && Kai Ye WUGSC All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef _REFSEQ_H_
#define _REFSEQ_H_

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "param.h"
#include "homo.h"
#include "bamreader.h"



// reference sequence information
struct RefTitle {
    RefTitle()
        : _name("")
        , _size(0)
    {
        //xxxx
    }
    std::string _name;
    bit32_t _size;
};

// RefSeq represent all the metadata outcoming from scanning a reference fasta file
class RefSeq
{
    public:

        RefSeq();
        ~RefSeq();
        
        // number of input sequence
        int total_num;
        // number of compute windows
        int totalWindows;

        // number of homopolymer sites
        int totalHomosites;
        // number of microsate sites
        int totalMicrosates; 
        // number of total sites
        int totalSites;

        //total length of all sequences
        bit64_t sum_length;

        std::vector< RefTitle > title;
        std::vector< HomoSite > homosBuffer;
        
        // for sites testing
        char chbuffer[MAX_FLANK_REGION];

        bool ifUserDefinedRegion;

        // Scan homosites and windows foreach reference sequence
        void ScanHomoAndMicrosate(std::ifstream &fin);
        // Scanning one reference sequence to find homopolymer/microsat
        void DoScan(int length, const std::string &name, unsigned int index);
        
        // Print output header
        bool PouroutHeader(std::ofstream &fout);
        
        // Print output and clear buffer
        bool PouroutBuffer(std::ofstream &fout);

        // redable testing sites function (not binary) NOT USED
        void TestSites();
        // testing sites binary function NOT USED
        void TestSitesBinary();

        // Instanciate one homo/microsate
        bit8_t LoadOneSite(const std::string & chr, const std::string & seq, int loc, bit8_t hLen, bit16_t type, bit16_t len, HomoSite & oneSite);
        
        // Load next sequence
        ref_loc_t LoadNextSeq(std::ifstream &fin);

        //void UnmaskRegion();

protected:

        std::string _name;
        std::string _seq;
        ref_loc_t _length;
        ref_id_t _count;
};

#endif //_REFSEQ_H_

