import sys

components = []
all_nodes = [i for i in range(1, 101)]
for line in sys.stdin:
    line = line.strip()
    u, v = line.split(' ')
    u, v = int(u), int(v)
    added=False
    for i in range(len(components)):
        if len(components[i].intersection({u, v})) > 0:
            components[i] = components[i].union({u, v})
            # all_nodes i slist of int
            if u in all_nodes:
                all_nodes.remove(u)
            if v in all_nodes:
                all_nodes.remove(v)
            added=True
            for j in range(i+1, len(components)):
                    if len(components[i].intersection(components[j])) > 0:
                        components[i] = components[i].union(components[j])
                        components[j] = set()
                        # This break statement can decreasr number of loops
                        # break
            break
    if not added:
        components.append({u, v})
        all_nodes.remove(u)
        all_nodes.remove(v)

for component in components:
    if len(component) > 0:
        component = [str(i) for i in component]
        print(' '.join(component))

for node in all_nodes:
    print(node)