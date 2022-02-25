#!/usr/bin/python
import os
import sys

if (len (sys.argv) != 7):
    print ("Usage: " + sys.argv[0] + " <jar file address> <input_file> <hdfs_input_dir> <hdfs_output_dir> <mapper_dir> <reducer_dir>")
    exit (1)

jar_file, input_file, hdfs_input_dir, hdfs_output_dir, mapper_dir, reducer_dir = sys.argv[1:]

# Set uid and gid to the current user
os.setuid(os.getuid())

# Delete both dirs
os.system('hdfs dfs -rm -R ' + hdfs_input_dir)
os.system('hdfs dfs -rm -R ' + hdfs_output_dir)

# create the input dir and copy the file
os.system('hdfs dfs -mkdir ' + hdfs_input_dir)
os.system('hdfs dfs -copyFromLocal ' + input_file + ' ' + hdfs_input_dir)

# get the input file name / or \ or //
input_file =  input_file.split('/')[-1]
input_file = input_file.split('\\')[-1]
input_file = input_file.split('//')[-1]
# Run mapreduce
final_command='hadoop jar ' + jar_file + ' -input ' + hdfs_input_dir + input_file + ' -output ' + hdfs_output_dir + ' -mapper ' + '"python3 ' + mapper_dir + 'mapper.py"' + ' -reducer ' + '"python3 ' + reducer_dir + 'reducer.py"'
print('command: ' + final_command)
os.system(final_command)