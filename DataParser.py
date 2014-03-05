import sys
import re

def buildDict(infile):
   wordOcc = {}
   wordList = re.split("[ , ,{,},\",/,=,_,(,),-,<,>,`,\',!,.,?,:,;,\n,,]", infile.read())
   
   for word in wordList:
      if word.isalpha():
         tempWord = word.capitalize()
         if tempWord in wordOcc:
            wordOcc[tempWord] += 1
         else:
            wordOcc[tempWord] = 1

   return wordOcc

def printHist(words, outfile):
   newDict = []
   toPrint = ""  
   for key, value in words.iteritems():
      temp = [key,value]
      newDict.append(temp)
   sortedDict = sorted(newDict, key=lambda word: word[1])
   for word, count in sortedDict:
      toPrint = word + ", " + str(count) + "\n" + toPrint
   outfile.write(toPrint)

def main():
   if len(sys.argv) < 2:
      sys.exit("Usage: " + sys.argv[0] + " <infile> <outfile>\n")
   infile = open(sys.argv[1], "r")
   outfile = open(sys.argv[2], "w")
   words = buildDict(infile)
   printHist(words, outfile)

main()
