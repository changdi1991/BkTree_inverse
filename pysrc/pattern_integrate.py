# Filename: pattern_integrate.py
# Author: Liang Ding
# Date: 9/16/2015

# Read a fasta sequence and its secondary structure. Generate a arff file that contains
# all bag motifs to be predicted by computed ANN models

__author__ = 'Liang Ding'

import itertools

class pattern_integrate:
    patFilePath = ''

    patOutPath = ''

    artificialNts = dict()

    allNtsLst = []

    artToNtsMap = dict()

    def __init__(self):
        self.patFilePath = ''
        self.patOutPath = ''
        self.artificialNts = dict()
        self.allNtsLst = []
        self.artToNtsMap = dict()
        return

    def gen_nt_compatible_matrix(self):
        self.__artificialNts()
        self.allNtsLst.append('A')
        self.artToNtsMap['A'] = 'A'
        self.allNtsLst.append('C')
        self.artToNtsMap['C'] = 'C'
        self.allNtsLst.append('G')
        self.artToNtsMap['G'] = 'G'
        self.allNtsLst.append('U')
        self.artToNtsMap['U'] = 'U'

        counter = 0
        for comb in itertools.combinations('ACGU', 2):
            s = ''
            for nt in comb:
                s += nt
            self.allNtsLst.append(str(counter))
            self.artToNtsMap[str(counter)] = s
            counter += 1
        for comb in itertools.combinations('ACGU', 3):
            s = ''
            for nt in comb:
                s += nt
            self.allNtsLst.append(str(counter))
            self.artToNtsMap[str(counter)] = s
            counter += 1
        self.allNtsLst.append('*')
        self.artToNtsMap['*'] = 'ACGU'

        # Order of each row is child nts, parent nts, is compatible or not,
        # child nts after merged, parent nts after merged
        fout = open('./nt_compatible_matrix.txt', 'w')
        for nt1 in self.allNtsLst:
            for nt2 in self.allNtsLst:
                fout.write(nt1 + ' ' + nt2 + ' ')
                if self.artToNtsMap[nt1] == self.artToNtsMap[nt2]:
                    fout.write('1 ' + nt1 + ' ' + nt2 + '\n')
                elif self.artToNtsMap[nt1] in self.artToNtsMap[nt2]:
                    fout.write('1 ' + nt1 + ' ' + nt1 + '\n')
                elif self.artToNtsMap[nt2] in self.artToNtsMap[nt1]:
                    fout.write('1 ' + nt2 + ' ' + nt2 + '\n')
                else:
                    commonNts = ''.join(set(self.artToNtsMap[nt1]).intersection(set(self.artToNtsMap[nt2])))
                    commonNts = ''.join(sorted(commonNts))
                    if commonNts != '':
                        fout.write('1 ' + self.artificialNts[commonNts] + ' ' + self.artificialNts[commonNts] + '\n')
                    else:
                        fout.write('0 none none\n')

        fout.close()
        return

    def __artificialNts(self):
        self.artificialNts['A'] = 'A'
        self.artificialNts['C'] = 'C'
        self.artificialNts['G'] = 'G'
        self.artificialNts['U'] = 'U'
        counter = 0
        for comb in itertools.combinations('ACGU', 2):
            s = ''
            for nt in comb:
                s += nt
            self.artificialNts[s] = str(counter)
            counter += 1
        for comb in itertools.combinations('ACGU', 3):
            s = ''
            for nt in comb:
                s += nt
            self.artificialNts[s] = str(counter)
            counter += 1
        self.artificialNts['ACGU'] = '*'
        #print self.artificialNts
        return

    def mergePattern(self, _patternLst):
        itemSplitedPats = []
        for pat in _patternLst:
            tokens = pat.split('[')
            nts = tokens[0]
            tokens2 = tokens[1].split(']')
            inters = tokens2[0]
            inters = inters.replace('n', '') # remove near interactions
            count = int(tokens2[1])
            #print (nts, inters, count)
            itemSplitedPats.append((nts, inters, count))

        self.__artificialNts()
        merged = []
        while len(itemSplitedPats) > 0:
            nts1 = itemSplitedPats[0][0]
            newNts = ''
            newPatCounter = itemSplitedPats[0][2]
            newItemSplitedPats = []
            tempNewNts = []
            for j in xrange(0, len(nts1)):
                tempNewNts.append(nts1[j])
            for i in xrange(1, len(itemSplitedPats)):
                nts2 = itemSplitedPats[i][0]
                if itemSplitedPats[0][1] == itemSplitedPats[i][1]:
                    for j in xrange(0, len(nts1)):
                        tempNewNts[j] += nts2[j]
                    newPatCounter += itemSplitedPats[i][2]
                else:
                    newItemSplitedPats.append(itemSplitedPats[i])
            for oneNts in tempNewNts:
                mergedNts = ''.join(sorted(''.join(set(oneNts))))
                if len(mergedNts) == 1:
                    newNts += mergedNts
                else:
                    newNts += self.artificialNts[mergedNts]
            merged.append((newNts, itemSplitedPats[0][1], newPatCounter))
            itemSplitedPats = newItemSplitedPats
            #break
        merged = sorted(merged, key=lambda pat: pat[2], reverse=True)
        #print merged
        return merged

    def toFile(self, _key, _mergedLst):
        fout = open(self.patOutPath, 'a')
        fout.write(_key + ' : ')
        for pat in _mergedLst:
            fout.write(pat[0])
            fout.write('[' + pat[1] + ']')
            fout.write(' ' + str(pat[2]) + ' ; ')
        fout.write('\n')
        fout.close()
        return

    def integrate(self):
        fin = open(self.patFilePath, 'r')
        fout = open(self.patOutPath, 'w')
        fout.close()
        while True:
            line = fin.readline()
            if not line:
                break
            tokens = line.split(' : ')
            key = tokens[0]
            patternLst = tokens[1].split(' ; ')
            patternLst.pop() # last pat is a new line character
            mergedPatLst = self.mergePattern(patternLst)
            print len(patternLst),
            print ' ',
            print len(mergedPatLst)
            #break
            self.toFile(key, mergedPatLst)
        fin.close()
        return

# Generate a compatible matrix for all nts including artificial nts for c++ program to read
def gen_nt_compatible_matrix():
    pi = pattern_integrate()
    pi.gen_nt_compatible_matrix()
    return

def main():
    pi = pattern_integrate()
    pi.patFilePath = '../out/2F8KB.pat'
    pi.patOutPath = '../out/2F8KB_merged.pat'
    pi.integrate()

if __name__ == '__main__':
    #main()
    gen_nt_compatible_matrix()

