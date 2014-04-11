import sys
import os
import re
import subprocess
import platform
import time
import codecs
from signal import *
from optparse import OptionParser

class Nlp_tweets:

	@staticmethod
	def  extract_ner(strTweet,strBASE_DIR):

		BASE_DIR = strBASE_DIR
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

		#if platform.architecture() != ('64bit', 'ELF'):
		#    sys.exit("Requires 64 bit Linux")

		start_time = time.time()

		parser = OptionParser()
		parser.add_option("--chunk", action="store_true", default=True)
		parser.add_option("--pos", action="store_true", default=True)
		parser.add_option("--classify", action="store_true", default=True)
		(options, args) = parser.parse_args()

		print(options.pos)

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
		tweet =strTweet
		#tweet =sys.stdin.readline().strip()
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
			#if nLines % 1000 == 0:
			#    start = time.time()
			#    ner.stdin.close()
			#    ner.stdout.close()
				#if ner.wait() != 0:
				#sys.stderr.write("error!\n")
				#ner.kill()
			 #   os.kill(ner.pid, SIGTERM)       #Need to do this for python 2.4
			#    ner.wait()
			#    ner = GetNer(ner_model)
		   # nLines += 1

		   # line =sys.stdin.readline().strip()
		   # line = line.encode('utf-8')
		  #  print(line)

		end_time = time.time()
		
		arr_nouns=extract_nouns(Output)
		london_loc=get_london_pre_defined_location(BASE_DIR)
		extracted_loc=extract_geolocation_nouns(arr_nouns,london_loc)
		strFullLocation=''
		if extracted_loc:
			
			if len(extracted_loc)>1:
				for i in range(len(extracted_loc)):
					if strFullLocation=='':
						strFullLocation=extracted_loc[i]
					else:
					     strFullLocation=strFullLocation+','+extracted_loc[i]
			else:
			     strFullLocation=extracted_loc[0]
				
		arr_words=extract_status_words(Output)

		arr_jammed_keywords=get_jammed_pre_defined_keywords(BASE_DIR)
		arr_crowded_keywords=get_crowded_pre_defined_keywords(BASE_DIR)
		arr_normal_keywords=get_normal_pre_defined_keywords(BASE_DIR)

		jam_occurance=extract_traffic_status(arr_words,arr_jammed_keywords)
		crowd_occurance=extract_traffic_status(arr_words,arr_crowded_keywords)
		normal_occurance=extract_traffic_status(arr_words,arr_normal_keywords)

		if jam_occurance>=normal_occurance or jam_occurance>=crowd_occurance :
			return 'extracted location:'+strFullLocation+'  status:jammed'
		elif normal_occurance>=jam_occurance or normal_occurance>=crowd_occurance :
			return 'extracted location:'+strFullLocation+'  status:normal'
		elif crowd_occurance>=jam_occurance or crowd_occurance>=normal_occurance :
			return 'extracted location:'+strFullLocation+'  status:crowded'
			

		#print "Average time per tweet = %ss" % (str((end_time-start_time) / nLines))
		#return extracted_loc

###############################################################################################################################3

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

	def extract_status_words(strOutput):

		pos_words={}
		j=0
		line = strOutput.split('\n')
		for i in range(len(line)):
			strLine=line[i]
			intIndex=strLine.rfind(':')+1
			strPos=strLine[intIndex:len(strLine)]
			if strPos=='NN' or strPos=='NNP' or strPos=='NNS' or strPos=='VBD' or strPos=='VBG' or strPos=='JJ':
			      intIndex2=strLine.find(':')+1
			      intIndex3=strLine.find('/')
			      strWord=strLine[intIndex2:intIndex3]
			      pos_words[j]=strWord
			      j+=1
		return pos_words

############################################################################################

	def get_london_pre_defined_location(strBASE_DIR):

		pre_defined_locations={}
		i=0
		for line in open('%s/Tweets_Data/London_Locations' % (strBASE_DIR)):       
			pre_defined_locations[i] = line.rstrip('\n')
			i+=1
		return pre_defined_locations

###############################################################################################

	def extract_geolocation_nouns(arr_nouns,arr_locations):
		extracted_locations={}
		x=0
		for i in range(len(arr_nouns)):
			strNoun=arr_nouns[i]
			strNoun=strNoun.lower()
			for j in range(len(arr_locations)):
				strLoc=arr_locations[j]
				intIndex=strLoc.find(strNoun)
				if intIndex!=-1:
					extracted_locations[x]=strNoun
					x+=1

		return extracted_locations

####################################################################################################

	def get_jammed_pre_defined_keywords(strBASE_DIR):
		pre_defined_Keywords={}
		i=0
		for line in open('%s/Tweets_Data/Jammed_Traffic' % (strBASE_DIR)):       
			pre_defined_Keywords[i] = line.rstrip('\n')
			i+=1
		return pre_defined_Keywords

##################################################################################################
				
	def get_crowded_pre_defined_keywords(strBASE_DIR):
		pre_defined_Keywords={}
		i=0
		for line in open('%s/Tweets_Data/Crowded_Traffic' % (strBASE_DIR)):       
			pre_defined_Keywords[i] = line.rstrip('\n')
			i+=1
		return pre_defined_Keywords

#################################################################################################

	def get_normal_pre_defined_keywords(strBASE_DIR):
		pre_defined_Keywords={}
		i=0
		for line in open('%s/Tweets_Data/Normal_Traffic' % (strBASE_DIR)):       
			pre_defined_Keywords[i] = line.rstrip('\n')
			i+=1
		return pre_defined_Keywords

###################################################################################################

	def extract_traffic_status(arr_tweet_words,arr_pre_defined_statua_keywords):
		arr_matched_keywords={}
		x=0
		for i in range(len(arr_tweet_words)):
			strTweetword=arr_tweet_words[i]
			strTweetword=strTweetword.lower()
			for j in range(len(arr_pre_defined_statua_keywords)):
				strKeyword=arr_pre_defined_statua_keywords[j]
				intIndex=strKeyword.find(strTweetword)
				if intIndex !=-1:
					arr_matched_keywords[x]=strTweetword
					x+=1
		return len(arr_matched_keywords)

##################################################################################################
##################################################################################################
def remove_redundancy (arr_values):
        arr_values_copy=arr_values
        for k,v in arr_values.items():
                strFirstword=v
                for k2,v2 in arr_values_copy.items():
                        strSecondword=v2
                        if strFirstword==strSecondword:
                                   del arr_values[k]
        return arr_values

####################################################################################################
def extract_estimated_locations(arr_all_tweets_words,arr_estimation_keywords):
        extracted_locations={}
        x=0
        for i in range(len(arr_all_tweets_words.keys())):
                strNoun=arr_all_tweets_words[i]
                strNoun=strNoun.lower()
                for j in range(len(arr_estimation_keywords)):
                        strEstimationKeyword=arr_estimation_keywords[j]
                        if strEstimationKeyword==strNoun:
                                 strEstimatedLocation=arr_all_tweets_words[(i + 1)]
                                 extracted_locations[x]=strEstimatedLocation
                                 x+=1

        return extracted_locations

				
