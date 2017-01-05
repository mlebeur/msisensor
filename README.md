MSIsensor
===========
MSIsensor is a C++ program for automatically detecting somatic variants at microsatellite regions. It computes length distributions of microsatellites per site in paired tumor and normal sequence data, subsequently using these to statistically compare observed distributions in both samples. Comprehensive testing indicates MSIsensor is an efficient and effective tool for deriving MSI status from standard tumor-normal paired sequence data.

Introduction
------------
Microsatellites/SSR/STR are repeated short DNA motifs. But so much on the "mathematical" definitions :

Motifs (pb)   |   Repetitions number   |   Homopolymer length (pb)   |   Source
- | :-: | :-: | -:
1-4 | / | / | [www.cancer.gov](https://www.cancer.gov/publications/dictionaries/cancer-terms?CdrID=285938)
2-8 | 100 | / | [www.ncbi.nlm.nih.gov](https://www.ncbi.nlm.nih.gov/mesh?Db=mesh&Cmd=DetailsSearch&Term=%22Microsatellite+Repeats%22%5BMeSH+Terms%5D)
2-5 | 5-50 | / | [https://en.wikipedia.org](https://en.wikipedia.org/wiki/Microsatellite)
1-6 | / | 10 | [http://gbe.oxfordjournals.org](http://gbe.oxfordjournals.org/content/2/620.full)
2-3 | 15-248 | / | [http://hgdownload.cse.ucsc.edu](http://hgdownload.cse.ucsc.edu/goldenPath/hg19/database/microsat.txt.gz)
1-1991 | 1.8-17326.4 | / | [http://hgdownload.cse.ucsc.edu](http://hgdownload.cse.ucsc.edu/goldenPath/hg19/database/simpleRepeat.txt.gz)

That's why, it can be really usefull to have a program able to detect all SSR present in a reference file according to the user definition.

Why SSR are so interesting ? After all, they are just non-coding reapeats dispersed all along the genome.

In fact the are intesting because the number of motif repetition can change due to [replication slippage](https://en.wikipedia.org/wiki/Replication_slippage).
But these repeat number variability is restricted by the [DNA repair mechanisms](https://en.wikipedia.org/wiki/DNA_repair). 
So if DNA repair mechanisms are defective then the repeat number variability should increase.

Futhermore, many diseases are linked to DNA repair mechanism deficiency such as cancer.
That's why, MicroSatellite Instability (MSI) was defined as an hypermutatbility revealing that DNA repair mechanism is not working normally.
Today, clinicians are looking for MSI using multi-PCR targeting 5-10 loci (ex : [https://www.google.com/patents/US7951564](https://www.google.com/patents/US7951564)).

But with personalized medicine incoming, we easily understand that the tomorow's challenge is to analyzed all the SSR present in Whole Exome Sequencing (WES) or RNA sequencing for each patient.

In conclusion, looking for SSR lenght variability allows the detection of DNA repair mechanism deficiency in patients.

PS : this information can help clinician for treatment guidance. For example, radiations treatment shouldn't be used on patient having already defective DNA repair mechanism, the immunotherapy seems to be a better solution.


Glossary
--------
* SSR/STR/Microsatellites are repeated short DNA motifs.
* MSI (MicroSatellite Instability) is an hypermutatbility revealing that DNA repair mechanism is not working normally.
* N means normal/WT (tissue, condition, file, ...).
* T means tumoral (tissue, condition, file, ...).
* Germline refer to elements (here SSR) having same length distribution in both N and T conditions.
* Somatic refer to elements (here SSR) having same length distribution in both N and T conditions.
* WES refer to [Whole Exome Sequencing](http://www.g3journal.org/content/5/8/1543.full).

Global mechanism
----------------
MSIsensor can :

1. Detect all SRR in a reference fasta file using the user's definition (msisensor scan).

To distinguish SSR (which are much more redondant all along the human genome) flanking regions must be extracted. For example :

        Grep TTTTTTTTTT hg19.fa         →      > 500 000 results
        Grep GTAGC (T)10 GGCTA hg19.fa  →      2 results
  

2. Compare all detected SSR between a normal BAM and tumoral BAM using their SSR length variability to identify SSR presenting MSI (msisensor msi).

In other words we want to compare length distributions of microsatellites per site in paired tumor and normal sequence data.

For a SSR I want to know if the numbers of repeats is the same between normal (N) and tumoral (T) DNA, so for each SSR we want a count table like that :

chr1 1560250 156256 (T)n 7

n | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8
- | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | -:
SSR | T | TT | TTT | TTTT | TTTTT | TTTTTT | TTTTTTT | TTTTTTTT
N | 0 | 0 | 0 | 0 | 0 | 1 | 9 | 0 
T | 0 | 1 | 8 | 7 | 1 | 0 | 0 | 0

This count table mean that 1 read containing the sequence TTTTTT was found in the region \[1560250-delta : 156256+delta\] of the normal BAM. In the same way, 9 reads containing the sequence TTTTTTT.
In the tumoral BAM 1 read containing TT, 8 reads containing TTT, 7 reads containing TTTT and 1 read containing TTTTT.

With the count table it's possible to use a chisquare test. But some axioms must be satisfied to use this test. For example the line count sum must be greater than 20 (foreach line). So the upper example will be filtered because the SSR is not enough covered in both Normal and Tumoral conditions. Let see with another example :

n | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8
- | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | -:
N | 0 | 0 | 0 | 0 | 0 | 0  | 20 | 0 
T | 0 | 0 | 0 | 0 | 0 | 20 | 0  | 0

This SSR is enough covered in both conditions, so the chisquare test can be performed. The corresponding p-value is 1.874e-09. This one is lower than 5% so we want to consider this SSR as somatic. But we are working with hundreds to thousands of data so a multiple correction test is needed. MSIsensor used benjamini hoechberg [FDR](https://en.wikipedia.org/wiki/False_discovery_rate).

So all SSR are processed, all p-values are then corrected. Finally, all p-values lower than 5% will be write in somatic file. All enough covered SSR wille be write in covered file and all read count tables will be write in dis file. 

Detailled mechanism
-------------------



Advices
-------

To me the best SSR definition is motif from 1 to 8 bp with at least 10 repeats. Fist because it incudes mostly all other definitions. 


Results
-------

* Detection :

Command dervied from adviced definition of SSR

        msisensor scan -d hg19.fa -o hg19.fa.msisensor -l 10 -m 100000 -s 8 -r 10
        
Only 6 minutes running with 0.5G of memory to find more thant 1 million of SSR.
        
* Comparaison :

Command dervied from adviced definition of SSR

        msisensor msi -d hg19.fa.msisensor -n normal_sorted.bam -t tumor_sorted.bam -o output -f 0.05 -c 20 -p 10 -m 100 -s 5 -w 6 -l 10 -q 1 -b 1

MSIsensor was tested on the MOSCATO essay. The results show that it really easy to distinguish patients having a MSI phenotype (verified by WET lab) from the others using only the percentage of MSI on the number of SSR enough covered.

I used a threshold of X for MOSCATO essay. But during my analysis I have shown that the inter-individual variability of SSR length is relatively high (like we are waiting for non coding sequences). So I beg you to perform your own validation and to choose your own threshold according your data before to determine the future of patients.

Usage
-----

        Version 0.2
        Usage:  msisensor <command> [options]

Key commands:

        scan            scan homopolymers and miscrosatelites
        msi             msi scoring

msisensor scan [options]:
       
       -d   <string>   reference genome sequences file, *.fasta format
       -o   <string>   output homopolymer and microsatelites file

       -l   <int>      minimal homopolymer size, default=5
       -c   <int>      context length, default=5
       -m   <int>      maximal homopolymer size, default=50
       -s   <int>      maximal length of microsatellite, default=5
       -r   <int>      minimal repeat times of microsatellite, default=3
       -p   <int>      output homopolymer only, 0: no; 1: yes, default=0
       
       -h   help
 
msisensor msi [options]:

       -d   <string>   homopolymer and microsatellites file
       -n   <string>   normal bam file ( bam index file is needed )
       -t   <string>   tumor  bam file ( bam index file is needed )
       -o   <string>   output distribution file

       -e   <string>   bed file, to select a few resions
       -f   <double>   FDR threshold for somatic sites detection, default=0.05 
       -r   <string>   choose one region, format: 1:10000000-20000000
       -l   <int>      mininal homopolymer size, default=5
       -p   <int>      mininal homopolymer size for distribution analysis, default=10
       -m   <int>      maximal homopolymer size for distribution analysis, default=50
       -q   <int>      mininal microsatellites size, default=3
       -s   <int>      mininal number of repeats in microsatellites for distribution analysis, default=5
       -w   <int>      maximal microsatellites size for distribution analysis, default=40
       -u   <int>      span size around window for extracting reads, default=500
       -b   <int>      threads number for parallel computing, default=1
       -x   <int>      output homopolymer only, 0: no; 1: yes, default=0
       -y   <int>      output microsatellite only, 0: no; 1: yes, default=0
       
       -h   help

Install
-------
The Makefile assumes that you have the samtools source code in an environment variable `$SAMTOOLS_ROOT`. 

you don't know what that means, then simply follow these steps from any directory that you have permissions to write into:
Install some prerequisite packages if you are using Debian or Ubuntu:

    sudo apt-get install git libbam-dev zlib1g-dev

If you are using Fedora, CentOS or RHEL, you'll need these packages instead:

    sudo yum install git samtools-devel zlib-devel

Download the samtools-0.1.19 from [SOURCEFORGE](http://sourceforge.net/projects/samtools/files/samtools/0.1.19):

    tar jxf samtools-0.1.19.tar.bz2
    cd samtools-0.1.19
    make
    export SAMTOOLS_ROOT=$PWD

Clone the msisensor repos, and build the `msisensor` binary:

    git clone https://github.com/ding-lab/msisensor.git
    cd msisensor
    make

Now you can put the resulting binary where your `$PATH` can find it. If you have su permissions, then
I recommend dumping it in the system directory for locally compiled packages:

    sudo mv msisensor /usr/local/bin/

We have also provided pre-build binary distributions for Linux x86_64 and Mac OS X in directory: ./binary

    msisensor_Linux_x86_64: for Linux x86_64
    msisensor_Mac_OS_X    : for Mac OS X


Example
-------
1. Scan microsatellites from reference genome:
  
        msisensor scan -d reference.fa -o microsatellites.list

2. Msi scorring: 

        msisensor msi -d microsatellites.list -n normal.bam -t tumor.bam -e bed.file -o output.prefix -l 1 -q 1 -b 2

   Note: normal and tumor bam index files are needed in the same directory as bam files 

Output
-------
There will be one microsatellite list output in "scan" step. Msi scorring step will give 4 output files based on given output prefix:
        microsatellites.list
        output
        output_dis
        output_covered
        output_somatic

1. microsatellites.list: microsatellite list output ( columns with *_binary means: binary conversion of DNA bases based on A=00, C=01, G=10, and T=11 )

        chromosome      location        repeat_unit_length     repeat_unit_binary    repeat_times    left_flank_binary     right_flank_binary      repeat_unit_bases      left_flank_bases       right_flank_bases
        1       10485   4       149     3       150     685     GCCC    AGCCG   GGGTC
        1       10629   2       9       3       258     409     GC      CAAAG   CGCGC
        1       10652   2       2       3       665     614     AG      GGCGC   GCGCG
        1       10658   2       9       3       546     409     GC      GAGAG   CGCGC
        1       10681   2       2       3       665     614     AG      GGCGC   GCGCG

2. output: msi score output (Global result on individual)

        Enough covered SSR number     Somatic SSR number      Percentage of somatic SSR on enough covered SSR
        640                                  75                       11.72

3. output_dis: read count distribution according SSR length (N: normal; T: tumor)

        1 10529896 CTTTC 15[T] GAGAC
        N: 0 0 0 0 0 0 0 1 0 0 8 9 1 7 17 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
        T: 0 0 0 0 0 0 0 0 0 1 19 14 17 9 32 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 

4. output_covered: SSR having enough coverage (both germline and somatic)
    
        chromosome   location        left_flank     repeat_times    repeat_unit_bases    right_flank      genotype
        1       1192105 AATAC   11      A       TTAGC   5|5
        1       1330899 CTGCC   5       AG      CACAG   5|5
        1       1598690 AATAC   12      A       TTAGC   5|5
        1       1605407 AAAAG   14      A       GAAAA   1|1
        1       2118724 TTTTC   11      T       CTTTT   1|1

5. output_somatic: somatic sites detected ( FDR: False Discovery Rate using BH algorithm) 

        chromosome   location        left_flank     repeat_times    repeat_unit_bases    right_flank      difference      P_value    FDR     rank
        1       16200729        TAAGA   10      T       CTTGT   0.55652 2.8973e-15      1.8542e-12      1
        1       75614380        TTTAC   14      T       AAGGT   0.82764 5.1515e-15      1.6485e-12      2
        1       70654981        CCAGG   21      A       GATGA   0.80556 1e-14   2.1333e-12      3
        1       65138787        GTTTG   13      A       CAGCT   0.8653  1e-14   1.6e-12 4
        1       35885046        TTCTC   11      T       CCCCT   0.84682 1e-14   1.28e-12        5
        1       75172756        GTGGT   14      A       GAAAA   0.57471 1e-14   1.0667e-12      6
        1       76257074        TGGAA   14      T       GAGTC   0.66023 1e-14   9.1429e-13      7
        1       33087567        TAGAG   16      A       GGAAA   0.53141 1e-14   8e-13   8
        1       41456808        CTAAC   14      T       CTTTT   0.76286 1e-14   7.1111e-13      9




Test sample
-----------
We provided one small sized sample data (tumor and matched normal bam files) for user to try msi scoring step.
It is very simple to run this test using sample data:

        cd ./test
        bash run.sh

Contact
-------
Please contact Lebeurrier Manuel manuel.lebeurrier@gustaveroussy.fr if you have any questions.

<Please contact Beifang Niu by bniu@genome.wustl.edu and Kai Ye by kye@genome.wustl.edu if you have any questions.>

Authors
-------

Beifang Niu

Kai Ye

Lebeurrier Manuel


Licence
-------

Until we choose a licence, please be advised that all rights are reserved.




