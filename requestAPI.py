import requests
import pandas as pd
import json
df = pd.read_csv('./dataRaw/truonghoc_add_add/truonghoc_add_add27.csv')
list_object = df[' Brainwave Value'].to_list()
print(list_object)
response = requests.post('http://8ca7-34-133-116-23.ngrok.io/api/predict', json={'list': list_object})
response = json.loads(response.text)
print(response['word'])
print(response['prob'])