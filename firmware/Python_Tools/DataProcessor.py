import shutil

arr=[]
arr = [0 for i in range(8192)] 
i = 0

with open('sample-data.txt') as f:
    while True:
        line = f.readline()
        if not line:
            break
        line = line.strip()
        data = float(line) * 25
        #print(data)
        arr[i] = data
        i += 1
f.close()

file_to_delete = open("processed-data.h",'w')
file_to_delete.close()

with open('processed-data.h', 'w') as d:
    d.write('float test_buffer[] = {')
    for data in arr:  
        d.write(str(data) + ',')
    d.write('};')
    d.close()

shutil.copyfile('processed-data.h', '/home/laurenz/Dokumente/GitHub/MTAP-MIDI-Guitar-Converter/firmware/ESP_DSP/src/processed-data.h')