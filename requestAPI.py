import requests
import pandas as pd
df = pd.read_csv('eeg_project/dataRaw/truonghoc_add_add/truonghoc_add_add27.csv')
list_object = df[' Brainwave Value'].to_list()
print(list_object)
response = requests.post('http://9966-34-124-161-212.ngrok.io/api/predict', json={'list': list_object})
print(response.text)