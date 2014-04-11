#!/usr/bin/python

import sys
import os
import re
import subprocess
import platform
import time
import codecs

from signal import *
import re
from optparse import OptionParser

#BASE_DIR = 'twitter_nlp.jar'

#if os.environ.has_key('TWITTER_NLP'):
#    BASE_DIR = os.environ['TWITTER_NLP']

BASE_DIR = '/home/fatma/twitter_data/twitter_nlp-master'
Output = " "
sys.path.append('%s/python' % (BASE_DIR))
sys.path.append('%s/python/ner' % (BASE_DIR))
sys.path.append('%s/hbc/python' % (BASE_DIR))

import Features
import twokenize
from LdaFeatures import LdaFeatures
from Dictionaries import Dictionaries
from Vocab import Vocab

sys.path.append('%s/python/cap' % (BASE_DIR))
sys.path.append('%s/python' % (BASE_DIR))
import cap_classifier
import pos_tagger_stdin
import chunk_tagger_stdin

def GetNer(ner_model):
        return subprocess.Popen('java -Xmx256m -cp %s/mallet-2.0.6/lib/mallet-deps.jar:%s/mallet-2.0.6/class cc.mallet.fst.SimpleTaggerStdin --weights sparse --model-file %s/models/ner/%s' % (BASE_DIR, BASE_DIR, BASE_DIR, ner_model),
                                           shell=True,
                                           close_fds=True,
                                           stdin=subprocess.PIPE,
                                           stdout=subprocess.PIPE)

def GetLLda():
        return subprocess.Popen('%s/hbc/models/LabeledLDA_infer_stdin.out %s/hbc/data/combined.docs.hbc %s/hbc/data/combined.z.hbc 100 100' % (BASE_DIR, BASE_DIR, BASE_DIR),
                                           shell=True,
                                           close_fds=True,
                                           stdin=subprocess.PIPE,
                                           stdout=subprocess.PIPE)
###############################################################################################################################3

#### function to extract only nouns from the tweets

def extract_nouns(strOutput):
    pos_nouns={}
    pos_words={}
    j=0
    line = strOutput.split('\n')
    for i in range(len(line)):
        strLine=line[i]
        intIndex=strLine.rfind(':')+1
        strPos=strLine[intIndex:len(strLine)]
        if strPos=='NN' or strPos=='NNP' or strPos=='NNS':
            intIndex2=strLine.find(':')+1
            intIndex3=strLine.find('/')
            strWord=strLine[intIndex2:intIndex3]
            pos_nouns[j]=strPos
            pos_words[j]=strWord
            j+=1
    return pos_words

##################################################################################################################################

###function to extract words refer to status from tweets

def extract_status_words(strOutput):
    pos_words={}
    j=0
    line = strOutput.split('\n')
    for i in range(len(line)):
        strLine=line[i]
        intIndex=strLine.rfind(':')+1
        strPos=strLine[intIndex:len(strLine)]
        if strPos=='NN' or strPos=='NNP' or strPos=='NNS' or strPos=='VBD' or strPos=='VBG' or strPos=='JJ' or strPos=='VBN' or strPos=='RB':
            intIndex2=strLine.find(':')+1
            intIndex3=strLine.find('/')
            strWord=strLine[intIndex2:intIndex3]
            pos_words[j]=strWord
            j+=1
    return pos_words

############################################################################################

##########function to extract words refer to location estimation from tweets

def extract_estimation_words(strOutput):
    pos_words={}
    j=0
    line = strOutput.split('\n')
    for i in range(len(line)):
        strLine=line[i]
        intIndex=strLine.rfind(':')+1
        strPos=strLine[intIndex:len(strLine)]
        if strPos=='IN' or strPos=='TO' or strPos=='JJ' or 'NNP':
            intIndex2=strLine.find(':')+1
            intIndex3=strLine.find('/')
            strWord=strLine[intIndex2:intIndex3]
            pos_words[j]=strWord
            j+=1
    return pos_words
##########function to extract "at" estimation keyword

def extract_estimation_tfl_words(strOutput,strTweet):
    strPart1=''
    strPart2=''
    tweetParts={}
    j=0
    line = strOutput.split('\n')
    for i in range(len(line)):
        strLine=line[i]
        intIndex=strLine.rfind(':')+1
        strPos=strLine[intIndex:len(strLine)]
        if strPos=='IN' or strPos=='TO' or strPos=='JJ' or 'NNP':
            intIndex2=strLine.find(':')+1
            intIndex3=strLine.find('/')
            strWord=strLine[intIndex2:intIndex3]
            if strWord=='at':
                 if strTweet.find(strWord):
                    pos_words[j]=strWord
                    j+=1
    return pos_words
##########function to extract estimated distances############################

def extract_estimation_distance(strOutput):

        pos_words={}
        j=0
        line = strOutput.split('\n')
        for i in range(len(line)):
                strLine=line[i]
                intIndex=strLine.rfind(':')+1
                strPos=strLine[intIndex:len(strLine)]
                if strPos=='IN' or strPos=='TO' or strPos=='JJ':
                          strLine=line[i+1]
                          intIndex=strLine.rfind(':')+1
                          strPos=strLine[intIndex:len(strLine)]
                          if strPos=='CD':
                            intIndex2=strLine.find(':')+1
                            intIndex3=strLine.find('/')
                            strWord=strLine[intIndex2:intIndex3]
                            pos_words[j]=strWord
                            j+=1
        return pos_words
############################################################################################

##########function to extract all words from tweets

def extract_all_words_from_tweet(strOutput):

        tweets_words={}
        j=0
        line = strOutput.split('\n')
        for i in range(len(line)):
                strLine=line[i]
                intIndex=strLine.rfind(':')+1
                strPos=strLine[intIndex:len(strLine)]
                if strPos!='':
                    intIndex=strLine.find(':')+1
                    intIndex2=strLine.find('/')
                    strWord=strLine[intIndex:intIndex2]
                    tweets_words[j]=strWord
                    j+=1
                
        return tweets_words

###############################################################################################

####function to get array of pre-defined locations from locations file in the sent directory

def get_london_pre_defined_location(strBASE_DIR):

        pre_defined_locations={}
        i=0
        for line in open('%s/Tweets_Data/Locations/London_Locations' % (strBASE_DIR)):
            pre_defined_locations[i] = line.rstrip('\n')
            i+=1
        return pre_defined_locations

###############################################################################################

####extract words from first array which occured in the second array

def extract_similar_nouns_from_tweets(arr_nouns,arr_locations):
        extracted_locations={}
        locations={}
        x=0
        for i in range(len(arr_nouns)):
                strNoun=arr_nouns[i]
                strNoun=strNoun.lower()
                for j in range(len(arr_locations)):
                        strLoc=arr_locations[j]
                        if strLoc==strNoun:
                                extracted_locations[x]=strNoun
                                x+=1
                                
        for k,v in extracted_locations.items():
                if v !='rd' and v !='ln' and v !='cnr' and v != 'road' and v !='lane' and v != 'corner' and v != 'pl' and v != 'place':
                        if v not in locations.values():
                                locations[k]=v
                else:
                      locations[k]=v  
                        
        return locations

####################################################################################################

####function to get array of pre-defined jammed traffic keywords from Jammed_Traffic file in the sent directory

def get_jammed_pre_defined_keywords(strBASE_DIR):
        pre_defined_Keywords={}
        i=0
        
        for line in open('%s/Tweets_Data/Traffic_Status/Jammed_Traffic' % (strBASE_DIR)):       
                pre_defined_Keywords[i] = line.rstrip('\n')
                i+=1
        return pre_defined_Keywords

##################################################################################################

####function to get array of pre-defined crowded traffic keywords from Crowded_Traffic file in the sent directory
                                
def get_crowded_pre_defined_keywords(strBASE_DIR):
        pre_defined_Keywords={}
        i=0
        for line in open('%s/Tweets_Data/Traffic_Status/Crowded_Traffic' % (strBASE_DIR)):       
                pre_defined_Keywords[i] = line.rstrip('\n')
                i+=1
        return pre_defined_Keywords

#################################################################################################

####function to get array of pre-defined normal traffic keywords from Normal_Traffic file in the sent directory

def get_normal_pre_defined_keywords(strBASE_DIR):
        pre_defined_Keywords={}
        i=0
        for line in open('%s/Tweets_Data/Traffic_Status/Normal_Traffic' % (strBASE_DIR)):       
                pre_defined_Keywords[i] = line.rstrip('\n')
                i+=1
        return pre_defined_Keywords

###################################################################################################
####function to get array of pre-defined london roads intersections

def get_intersections(strBASE_DIR):
        pre_defined_Keywords={}
        i=0
        for line in open('%s/Tweets_Data/Locations/London_Intersections' % (strBASE_DIR)):       
                pre_defined_Keywords[i] = line.rstrip('\n')
                i+=1
        return pre_defined_Keywords

###################################################################################################
####function to get array of pre-defined london roads segmnts

def get_segmnts(strBASE_DIR):
        pre_defined_Keywords={}
        i=0
        for line in open('%s/Tweets_Data/Locations/road_segmnts' % (strBASE_DIR)):       
                pre_defined_Keywords[i] = line.rstrip('\n')
                i+=1
        return pre_defined_Keywords

###################################################################################################

####function to get array of pre-defined directions keywords from directions file in the sent directory

def get_dirctions_keywords(strBASE_DIR):
        pre_defined_Keywords={}
        i=0
        for line in open('%s/Tweets_Data/Location_Estimation/directions' % (strBASE_DIR)):       
                pre_defined_Keywords[i] = line.rstrip('\n')
                i+=1
        return pre_defined_Keywords

###################################################################################################

####function to get array of pre-defined estimations keywords from estimations file in the sent directory

def get_estimation_keywords(strBASE_DIR):
        pre_defined_Keywords={}
        i=0
        for line in open('%s/Tweets_Data/Location_Estimation/estimations' % (strBASE_DIR)):       
                pre_defined_Keywords[i] = line.rstrip('\n')
                i+=1
        return pre_defined_Keywords

###################################################################################################

##################function to get the array of pre-defined keywords from causes file#############################

def get_causes_keywords(strBASE_DIR):
        pre_defined_Keywords={}
        i=0
        for line in open('%s/Tweets_Data/Traffic_Status/causes' % (strBASE_DIR)):       
                pre_defined_Keywords[i] = line.rstrip('\n')
                i+=1
        return pre_defined_Keywords


## function to get the lenght of the of array for the occured keywords in the tweets

def get_the_no_occurances_of_keywords_in_tweets(arr_tweet_words,arr_pre_defined_statua_keywords):
        arr_matched_keywords={}
        x=0
        for i in range(len(arr_tweet_words)):
                strTweetword=arr_tweet_words[i]
                strTweetword=strTweetword.lower()
                for j in range(len(arr_pre_defined_statua_keywords)):
                        strKeyword=arr_pre_defined_statua_keywords[j]
                        if strKeyword ==strTweetword:
                                arr_matched_keywords[x]=strTweetword
                                x+=1
        return len(arr_matched_keywords)

##################################################################################################
###extract the estimated locations (which are the words right after the estimation keywords) in the tweets

def extract_estimated_locations(arr_all_tweets_words,arr_estimation_keywords):
        extracted_locations={}
        extracted_locations2={}
        x=0
        for i in range(len(arr_all_tweets_words.keys())):
                strNoun=arr_all_tweets_words[i]
                strNoun=strNoun.lower()
                for j in range(len(arr_estimation_keywords)):
                        strEstimationKeyword=arr_estimation_keywords[j]
                        if strEstimationKeyword==strNoun:
                                 str1=arr_all_tweets_words[(i + 1)]
                                 str1=str1.lower()
                                 str2=arr_all_tweets_words[(i + 2)]
                                 str2=str2.lower()                          
                                 strEstimatedLocation=str1+' '+str2
                                 extracted_locations[x]=strEstimatedLocation
                                 x+=1

        for k,v in extracted_locations.items():
                 if v not in extracted_locations2.values():
                        extracted_locations2[k]=v                        

        return extracted_locations2
#####################################3 ###########################################
def extract_estimatation_string(arr_all_tweets_words,arr_estimation_keywords):
        strEstimationSubString='';
        strLine=str(arr_all_tweets_words)
        for i in range(len(arr_all_tweets_words)):
                strNoun=arr_all_tweets_words[i]
                strNoun=strNoun.lower()
                for j in range(len(arr_estimation_keywords)):
                        strEstimationKeyword=arr_estimation_keywords[j]
                        if strEstimationKeyword==strNoun: 
                            strEstimationSubString=strLine[i:len(arr_all_tweets_words)]
        return strEstimationSubString

#########################################################################################################3        

#if platform.architecture() != ('64bit', 'ELF'):
#    sys.exit("Requires 64 bit Linux")

start_time = time.time()

parser = OptionParser()
parser.add_option("--chunk", action="store_true", default=True)
parser.add_option("--pos", action="store_true", default=True)
parser.add_option("--classify", action="store_true", default=True)
(options, args) = parser.parse_args()


if options.pos:
        posTagger = pos_tagger_stdin.PosTagger()
else:
        posTagger = None

if options.chunk and options.pos:
        chunkTagger = chunk_tagger_stdin.ChunkTagger()
else:
        chunkTagger = None

if options.classify:
        llda = GetLLda()
else:
        llda = None

if options.pos and options.chunk:
        ner_model = 'ner.model'
elif options.pos:
        ner_model = 'ner_nochunk.model'
else:
        ner_model = 'ner_nopos_nochunk.model'

ner = GetNer(ner_model)
fe = Features.FeatureExtractor('%s/data/dictionaries' % (BASE_DIR))

capClassifier = cap_classifier.CapClassifier()

vocab = Vocab('%s/hbc/data/vocab' % (BASE_DIR))

dictMap = {}
i = 1
for line in open('%s/hbc/data/dictionaries' % (BASE_DIR)):
        dictionary = line.rstrip('\n')
        dictMap[i] = dictionary
        i += 1

dict2index = {}
for i in dictMap.keys():
        dict2index[dictMap[i]] = i

if llda:
        dictionaries = Dictionaries('%s/data/LabeledLDA_dictionaries3' % (BASE_DIR), dict2index)
entityMap = {}
i = 0
if llda:
        for line in open('%s/hbc/data/entities' % (BASE_DIR)):
                entity = line.rstrip('\n')
                entityMap[entity] = i
                i += 1

dict2label = {}
for line in open('%s/hbc/data/dict-label3' % (BASE_DIR)):
        (dictionary, label) = line.rstrip('\n').split(' ')
        dict2label[dictionary] = label

nLines = 1
#tweet ='Accident - A4 Great West Rd about 100 M from Syon Lane near Gillette Cnr  - Road has been opened with lane restricted.'+'\n'+'Correction Accident - A4 Great West Rd at Syon Lane (Gillette Cnr) Road was closed w/b, which has been opened with a lane one restriction.' 
#tweet ='Shepherds Bush Green remains closed for gasworks from Holland Park RBT, diversions in place - expect peak period delays (espec. Holland Rd)\nShepherds Bush Green is closed towards King Street College due to roadworks between Holland Rd Roundabout and West 12 Shopping Centre\nR.I.P to the possum on Holland Rd that car crushed you.Kroger making a comeback the one on holland rd gon be nice as hell\n@TeaQ09 @Moe_Diesel_Baby  Whats going on in wagener on new holland rd????'
#tweet='The A4 Ellesmere Rd has reopened at Sutton Court Rd following the earlier collision. Residual Qs remain back to junction 2 on the M4'
tweet =sys.stdin.readline().strip()
line = tweet.encode('utf-8')
while line:
        words = twokenize.tokenize(line)
        seq_features = []
        tags = []

        goodCap = capClassifier.Classify(words) > 0.9

        if posTagger:
                pos = posTagger.TagSentence(words)
                pos = [p.split(':')[0] for p in pos]  # remove weights   
        else:
                pos = None

        # Chunking the tweet
        if posTagger and chunkTagger:
                word_pos = zip(words, [p.split(':')[0] for p in pos])
                chunk = chunkTagger.TagSentence(word_pos)
                chunk = [c.split(':')[0] for c in chunk]  # remove weights      
        else:
                chunk = None

        quotes = Features.GetQuotes(words)
        for i in range(len(words)):
                features = fe.Extract(words, pos, chunk, i, goodCap) + ['DOMAIN=Twitter']
                if quotes[i]:
                        features.append("QUOTED")
                seq_features.append(" ".join(features))
        ner.stdin.write(("\t".join(seq_features) + "\n").encode('utf8'))
                
        for i in range(len(words)):
                tags.append(ner.stdout.readline().rstrip('\n').strip(' '))

        features = LdaFeatures(words, tags)

        #Extract and classify entities
        for i in range(len(features.entities)):
                type = None
                wids = [str(vocab.GetID(x.lower())) for x in features.features[i] if vocab.HasWord(x.lower())]
                if llda and len(wids) > 0:
                        entityid = "-1"
                        if entityMap.has_key(features.entityStrings[i].lower()):
                                entityid = str(entityMap[features.entityStrings[i].lower()])
                        labels = dictionaries.GetDictVector(features.entityStrings[i])

                        if sum(labels) == 0:
                                labels = [1 for x in labels]
                        llda.stdin.write("\t".join([entityid, " ".join(wids), " ".join([str(x) for x in labels])]) + "\n")
                        sample = llda.stdout.readline().rstrip('\n')
                        labels = [dict2label[dictMap[int(x)]] for x in sample[4:len(sample)-8].split(' ')]

                        count = {}
                        for label in labels:
                                count[label] = count.get(label, 0.0) + 1.0
                        maxL = None
                        maxP = 0.0
                        for label in count.keys():
                                p = count[label] / float(len(count))
                                if p > maxP or maxL == None:
                                        maxL = label
                                        maxP = p

                        if maxL != 'None':
                                tags[features.entities[i][0]] = "B-%s" % (maxL)
                                for j in range(features.entities[i][0]+1,features.entities[i][1]):
                                        tags[j] = "I-%s" % (maxL)
                        else:
                                tags[features.entities[i][0]] = "O"
                                for j in range(features.entities[i][0]+1,features.entities[i][1]):
                                        tags[j] = "O"
                else:
                        tags[features.entities[i][0]] = "B-ENTITY"
                        for j in range(features.entities[i][0]+1,features.entities[i][1]):
                                tags[j] = "I-ENTITY"

        if pos:
                #sys.stdout.write((" ".join(["%s/%s/%s" % (words[x], tags[x], pos[x]) for x in range(len(words))]) + "\n").encode('utf8'))
                                Output=Output+("\n".join(["word:%s/tag:%s/pos:%s" % (words[x], tags[x], pos[x])  for x in range(len(words))]) + "\n").encode('utf8')
        else:
                sys.stdout.write((" ".join(["%s/%s" % (words[x], tags[x]) for x in range(len(words))]) + "\n").encode('utf8'))        
                 
        line=''
        #sys.stdout.write(Output)
        #sys.stdout.flush()
        #seems like there is a memory leak comming from mallet, so just restart it every 1,000 tweets or so
        if nLines % 1000 == 0:
                 ner.stdin.close()
                 ner.stdout.close()
                #if ner.wait() != 0:
                #sys.stderr.write("error!\n")
                #ner.kill()
                 os.kill(ner.pid, SIGTERM)       #Need to do this for python 2.4
                 ner.wait()
                 ner = GetNer(ner_model)
        nLines += 1
        
        line = sys.stdin.readline().strip()
        line = line.encode('utf-8')

######################################################CALL the Geo-Location Part################################
arr_result={}
  
arr_nouns=extract_nouns(Output)
london_loc=get_london_pre_defined_location(BASE_DIR)
extracted_loc=extract_similar_nouns_from_tweets(arr_nouns,london_loc)
no=len(extracted_loc);
strFullLocation=''
if len(extracted_loc)>0:
                
        if len(extracted_loc)>1:
                 #for i in range(len(extracted_loc)):
                 for k,v in extracted_loc.items():        
                        strExtractedLoc=extracted_loc[k]
                        if strFullLocation=='':
                                strFullLocation=strExtractedLoc
                        else:
                                if k !=(len(extracted_loc)):
                                        if strExtractedLoc=='rd' or strExtractedLoc=='ln' or strExtractedLoc=='cnr' or strExtractedLoc=='road' or strExtractedLoc=='lane' or strExtractedLoc=='corner' or strExtractedLoc=='pl' or strExtractedLoc=='place' :
                                               strFullLocation=strFullLocation+' '+strExtractedLoc+'/'
                                        else:
                                                strFullLocation=strFullLocation+' '+strExtractedLoc
                                                
                                else:
                                       strFullLocation=strFullLocation+' '+strExtractedLoc 
        else:
                strFullLocation=strExtractedLoc
               

else:
        strFullLocation='No Location specified'

strModifiedLocation=strFullLocation
strResult=''
road_segmnts=get_segmnts(BASE_DIR)
 
if strModifiedLocation.find('/')!= -1:     
        strModifiedLocation=strModifiedLocation.split("/")       
        if len(strModifiedLocation)>0:
                for k in range(len(strModifiedLocation)):
                         strAny=strModifiedLocation[k]
                         m = re.search("\d",strAny )           
                         if m is not None:
                               # m= m.start()
                               # strModifiedLocation[k] = list(strModifiedLocation[k])
                               #strModifiedLocation[k].insert(m+1, ',') #insert the ':' character into the list before position 2
                               # strModifiedLocation[k] = "".join(strModifiedLocation[k])

                                #check if this road segment is in hte road_segmnts file

                                for k2,v2 in road_segmnts.items():
                                        strSegmnt=road_segmnts[k2]
                                        intIndex=strSegmnt.find("[")
                                        intIndex2=strSegmnt.find("]")
                                        strSegmntStatment=strSegmnt[0:intIndex]
                                        strSegmntLocation=strSegmnt[intIndex+1:intIndex2]
                                        if strSegmntStatment.find(strModifiedLocation[k]) !=-1:
                                                strResult=strSegmntLocation
            
                                if strResult=='':
                                          strResult=strModifiedLocation[k]
                                else:
                                        strResult=strResult+'/'
                          
                         else:
                                
                                  if strResult=='':
                                          strResult=strModifiedLocation[k]
                                  else:
                                          strResult=strResult+'/'+strModifiedLocation[k]
                  
else:
        m = re.search("\d", strModifiedLocation)
        m=m.start()
        if m is not None:
                
                #strModifiedLocation= list(strModifiedLocation)
                #strModifiedLocation.insert(m+1, ',') #insert the ':' character into the list before position 2
                #strModifiedLocation = "".join(strModifiedLocation)
                for k2,v2 in road_segmnts.items():
                        strSegmnt=road_segmnts[k2]
                        intIndex=strSegmnt.find("[")
                        intIndex2=strSegmnt.find("]")
                        strSegmntStatment=strSegmnt[0:intIndex]
                        strSegmntLocation=strSegmnt[intIndex+1:intIndex2]
                        if strSegmntStatment.find(strModifiedLocation) !=-1:
                                strResult=strSegmntLocation
                                         
                if strResult=='':
                          strResult=strModifiedLocation[k]
                else:
                         strResult=strResult+'/'
          
        else:
                
                  if strResult=='':
                          strResult=strModifiedLocation[k]
                  else:
                          strResult=strResult+'/'+strModifiedLocation[k]


        
#strFullLocation=strFullLocation
arr_result[0]=strResult
        
######################################################################################Call the estimated locations part#################33

arr_tweets_estimation_words=extract_estimation_words(Output)
strEstimatedLocation=''
no_occurance=0
#check if there is 'at' key word in arr_tweets_estimation_words dictionary
if  'at' in arr_tweets_estimation_words.values():
        if strFullLocation.find('/')!= -1:
                locations=strFullLocation.split("/")
                for i in range(len(locations)):
                        strany=locations[i]
                        #check if the tweet contain the intersection files
                        london_intersections=get_intersections(BASE_DIR)
                        for k,v in london_intersections.items():
                                strIntersection=london_intersections[k]
                                intIndex=strIntersection.find("[")
                                intIndex2=strIntersection.find("]")
                                strIntersectionStatment=strIntersection[0:intIndex]
                                strIntersectionLocation=strIntersection[intIndex+1:intIndex2]
                                if strIntersectionStatment.find(locations[i])!=-1:
                                        no_occurance=no_occurance+1
                                        if no_occurance>=2:
                                                strEstimatedLocation=strIntersectionLocation
                
                        
                
else:
        strEstimatedLocation='No intersections found'
        
arr_result[1]=strEstimatedLocation

##strEstimatedLocation=''
##strExtractedEstimation=''
##if len(arr_estimated_locations)>0:
##         if len(arr_estimated_locations)>1:
##                #for i in range(len(arr_estimated_locations)):
##                for k,v in arr_estimated_locations.items():
##                        strExtractedEstimation=arr_estimated_locations[k]
##                        if strEstimatedLocation=='':
##                                if i != (len(arr_estimated_locations)-1):
##                                        if strExtractedEstimation.find('rd') or strExtractedEstimation.find('ln') or strExtractedEstimation.find('cnr') or strExtractedEstimation.find('road') or strExtractedEstimation.find('lane') or strExtractedEstimation.find('corner')or strExtractedEstimation.find('pl') or strExtractedEstimation.find('place'):
##                                                strEstimatedLocation=strExtractedEstimation+'/'
##                                        else:
##                                               strEstimatedLocation=strExtractedEstimation
##                                else:
##                                       strEstimatedLocation=strExtractedEstimation
##                        else:
##                                if i != (len(arr_estimated_locations)-1):
##                                         if strExtractedEstimation.find('rd') or strExtractedEstimation.find('ln') or strExtractedEstimation.find('cnr') or strExtractedEstimation.find('road') or strExtractedEstimation.find('lane') or strExtractedEstimation.find('corner') or strExtractedEstimation.find('pl') or strExtractedEstimation.find('place'):
##                                                strEstimatedLocation=strEstimatedLocation+strExtractedEstimation+'/'
##                                         else:
##                                                 strEstimatedLocation=strEstimatedLocation+strExtractedEstimation
##                                else:
##                                      strEstimatedLocation=strEstimatedLocation+strExtractedEstimation 
##                                        
##         else:
##                strEstimatedLocation=strExtractedEstimation
##else:
##        strEstimatedLocation='No estimation specified'        
##
###strEstimatedLocation=strEstimatedLocation
##arr_result[1]=strEstimatedLocation
############################################### call the Estimated Distance function#############################
arr_estimated_distance=extract_estimation_distance(Output)

strEstimatedDistance=''
if len(arr_estimated_distance)>0:
         if len(arr_estimated_locations)>1:
                for i in range(len(arr_estimated_distance)):
                        if strEstimatedDistance=='':
                                strEstimatedDistance=arr_estimated_distance[i]
                        else:
                                  strEstimatedDistance=strEstimatedLocation+'/'+arr_estimated_distance[i]
         else:
                strEstimatedDistance=arr_estimated_distance[0]
else:
        strEstimatedDistance='No estimated distance specified'     

#arr_result[2]=strEstimatedDistance
#######################################################Call The traffic status Part########################33                           
arr_words=extract_status_words(Output)

arr_jammed_keywords=get_jammed_pre_defined_keywords(BASE_DIR)
arr_crowded_keywords=get_crowded_pre_defined_keywords(BASE_DIR)
arr_normal_keywords=get_normal_pre_defined_keywords(BASE_DIR)

jam_occurance=get_the_no_occurances_of_keywords_in_tweets(arr_words,arr_jammed_keywords)
crowd_occurance=get_the_no_occurances_of_keywords_in_tweets(arr_words,arr_crowded_keywords)
normal_occurance=get_the_no_occurances_of_keywords_in_tweets(arr_words,arr_normal_keywords)
strTrafficStatus=''

if jam_occurance==0 and crowd_occurance==0 and normal_occurance==0:
        strTrafficStatus='No specified traffic status'
        
else:
        if jam_occurance>=normal_occurance and jam_occurance>=crowd_occurance :
                #print 'extracted location:'+strFullLocation+'  status:jammed'
                 strTrafficStatus='jammed'
        elif crowd_occurance>jam_occurance and crowd_occurance>=normal_occurance :
                #print 'extracted location:'+strFullLocation+'  status:crowded'
                 strTrafficStatus='crowded'
        elif normal_occurance>jam_occurance and normal_occurance>crowd_occurance :
                #print 'extracted location:'+strFullLocation+'  status:normal'
                 strTrafficStatus='normal'
  
arr_result[2]=strTrafficStatus                 
####################################################################################Call the directions keyword#####################

#arr_directions_keywords=get_dirctions_keywords(BASE_DIR)
#extracted_direction=extract_similar_nouns_from_tweets(arr_nouns,arr_directions_keywords)
#directions_occurance=get_the_no_occurances_of_keywords_in_tweets(arr_nouns,arr_directions_keywords)

#if len(extracted_direction)>0:
           #strExtractedDirection=arr_extracted_directions[0]
#else:
        #strExtractedDirection='No specified direction'

#arr_result[2]=strExtractedDirection


#######################################################################################################################################        
strCause=''
arr_causes_keywords=get_causes_keywords(BASE_DIR)  
extracted_cause=extract_similar_nouns_from_tweets(arr_nouns,arr_causes_keywords)
if len(extracted_cause)>0:
    strCause=extracted_cause[0]
else:
     strCause='No specified cause'   
    
arr_result[3]=strCause

print(arr_result[0]+'\n'+arr_result[1]+'\n'+arr_result[2]+'\n'+arr_result[3])
