import csv
with open('dataRaw/datnuoc_add_add/datnuoc_add_add1.csv') as originFile:
    reader=csv.reader(originFile)
    rows=[row[2] for row in reader]
with open('datnuoc.csv','w', newline='') as new_file:
    writer=csv.writer(new_file)
    for row in rows:
        writer.writerow([row])