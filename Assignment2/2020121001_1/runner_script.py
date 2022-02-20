#!/usr/bin/python
import os
import sys

if (len (sys.argv) != 7):
    print ("Usage: " + sys.argv[0] + " <jar file address> <input_file> <hdfs_input_dir> <hdfs_output_dir> <mapper_dir> <reducer_dir>")
    exit (1)

jar_file, input_file, hdfs_input_dir, hdfs_output_dir, mapper_dir, reducer_dir = sys.argv[1:]

lines = []
newlines = []
with open(input_file, 'r') as f:
    lines=f.readlines()

pre='A'
x1 = int(lines[0].split(' ')[0])
x2 = int(lines[x1+1].split(' ')[0])
newlines.append('AR ' + lines[0].strip() )
for i in range(1, x1 + 1):
    j=0
    for line in lines[i].strip().split():
        v = pre + ' ' + str(i-1) + ' ' + str(j) + ' ' + line.strip()
        newlines.append(v)
        j+=1
pre='B'
newlines.append('BR ' + lines[x1+1].strip())
for i in range(x1+2,x1+x2+2):
    j=0
    for line in lines[i].split():
        v = pre + ' ' + str(i - x1 - 2) + ' ' + str(j) + ' ' + line.strip()
        newlines.append(v)
        j+=1


input_file =  input_file.split('/')[-1]
input_file = input_file.split('\\')[-1]
input_file = input_file.split('//')[-1]

# write the new file
with open('mod_' + input_file, 'w') as f:
    for line in newlines:
        f.write(line + '\n')

# Set uid and gid to the current user
os.setuid(os.getuid())

# Delete both dirs
os.system('hdfs dfs -rm -R ' + hdfs_input_dir)
os.system('hdfs dfs -rm -R ' + hdfs_output_dir)

# create the input dir and copy the file
os.system('hdfs dfs -mkdir ' + hdfs_input_dir)
os.system('hdfs dfs -copyFromLocal mod_' + input_file + ' ' + hdfs_input_dir)

# Run mapreduce
final_command='hadoop jar ' + jar_file + ' -input ' + hdfs_input_dir + 'mod_' + input_file + ' -output ' + hdfs_output_dir + ' -mapper ' + '"python3 ' + mapper_dir + 'mapper.py"' + ' -reducer ' + '"python3 ' + reducer_dir + 'reducer.py"'
print('command: ' + final_command)
os.system(final_command)
