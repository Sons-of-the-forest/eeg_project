from os import listdir
from os.path import isfile, join
import os
import csv
import pandas as pd

rawdataBasePath='dataRaw'
newRawdataBasePath='dataRaw'
wordList = next(os.walk(rawdataBasePath))[1]
for word in wordList:
    wordDataPath=os.path.join(rawdataBasePath,word)
    wordFiles = [wordFile for wordFile in listdir(wordDataPath) if isfile(join(wordDataPath, wordFile))]
    for wordFile in wordFiles:
        path=os.path.join(wordDataPath,wordFile)
        print(path)
        df = pd.read_csv(path)
        count_row = df.shape[0]
        for i in range(count_row):
            brainwaveData=df.iloc[i,2]
            brainwaveData=int(brainwaveData)
            if brainwaveData>=32768:
                brainwaveData=brainwaveData-65536
                df.iloc[i,2]=brainwaveData
        newWordDataPath=os.path.join(newRawdataBasePath,word)
        if not os.path.isdir(newWordDataPath):
            os.makedirs(newWordDataPath, 0o666) 
        df.to_csv(os.path.join(newWordDataPath,wordFile), index=False)
