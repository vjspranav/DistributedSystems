
#!/usr/bin/python3
import sys

for line in sys.stdin:
    key,val = line.strip().split()
    # the smaller one is key
    if int(key) > int(val):
        key, val = val, key
    print('%s %s' % (key, val))
