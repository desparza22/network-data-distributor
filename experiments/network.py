from matplotlib import pyplot as plt
from matplotlib import animation
import random
import math
import copy

class Node:
    max_height = 0
    
    def __init__(self, x: int, y: int):
        self.x = x
        self.y = y
        self.upstream = 0
        self.downstream = []
        self.height = 0

class Package:
    def __init__(self, x: int, y: int, id_num: int):
        self.x = x
        self.y = y
        self.id_num = id_num


        
height = 7
width = 9
x_width = width*10
y_height = height*10

#packages start out present at the head. each package has a "center", and each node
#requests packages at a frequency relative to how close it is to the package's center
num_packages_root = 4
package_radius = 1.5
package_color = (1, 1, 0)
x_dist = x_width // num_packages_root
y_dist = y_height // num_packages_root
x_current = x_dist // 2
packages = []
for i in range(num_packages_root):
    y_current = y_dist // 2
    for j in range(num_packages_root):
        packages.append(Package(x_current, y_current, j + i * num_packages_root))
        y_current += y_dist
    x_current += x_dist

nodes = []
radius = 1.2
int_radius = math.ceil(radius)
num_circles = 25
for _ in range(num_circles):
    x_coord = random.randint(int_radius, x_width - int_radius)
    y_coord = random.randint(int_radius, y_height - int_radius)
    node = Node(x_coord, y_coord)
    nodes.append(node)

def construct_tree(nodes: list[Node],
                   xy_bounds: list[list[int]],
                   height: int) -> Node:
    x_low, x_high, y_low, y_high = \
        xy_bounds[0][0], xy_bounds[0][1], \
        xy_bounds[1][0], xy_bounds[1][1]
    x_center, y_center = calc_centers(nodes)

    min_dist = 5 * ((x_high - x_low)**2 + (y_high - y_low)**2)
    center_node_idx = -1
    for index in range(len(nodes)):
        node = nodes[index]
        compare_dist = (node.x - x_center)**2 + (node.y - y_center)**2
        if compare_dist < min_dist:
            min_dist = compare_dist
            center_node_idx = index

    #print(f"index: {center_node_idx} of {len(nodes)}")
    center_node = nodes[center_node_idx]
    center_node.height = height
    nodes = copy.copy(nodes)
    popped = nodes.pop(center_node_idx)

    #print(f"popped: {popped.x} {popped.y}, center: {center_node.x}, {center_node.y}")
    
    node_groups = group_nodes(nodes)

    center_node.height = height
    if height > Node.max_height:
        Node.max_height = height
    
    if len(node_groups) == 1:
        #print(f"adding leaves, {len(node_groups[0])}")
        for child in node_groups[0]:
            child.upstream = center_node
            center_node.downstream.append(child)
            child.height = height+1
            if child.height > Node.max_height:
                Node.max_height = child.height

    else:
        child_bounds = [[[x_low, x_center], [y_center, y_high]],
                        [[x_low, x_center], [y_low, y_center]],
                        [[x_center, x_high], [y_center, y_high]],
                        [[x_center, x_high], [y_low, y_center]]]
        
        for group_idx in range(len(node_groups)):
            group = node_groups[group_idx]
            bounds = child_bounds[group_idx]
            
            child = construct_tree(group, bounds, height+1)
            child.upstream = center_node
            center_node.downstream.append(child)

    return center_node

def calc_centers(nodes: list[Node]) -> tuple[int, int]:
    x_sum = 0
    y_sum = 0
    for node in nodes:
        x_sum += node.x
        y_sum += node.y
    return x_sum // len(nodes), y_sum // len(nodes)

def group_nodes(nodes: list[Node]) -> list[list[Node]]:
    if len(nodes) < 6:
        return [nodes]

    nodes = copy.copy(nodes)
    nodes.sort(key = lambda node: node.x)
    left = nodes[:len(nodes)//2]
    right = nodes[len(nodes)//2:]

    left.sort(key = lambda node: node.y)
    right.sort(key = lambda node: node.y)
    top_left = left[len(left)//2:]
    bottom_left = left[:len(left)//2]
    top_right = right[len(right)//2:]
    bottom_right = right[:len(right)//2]

    return [top_left, bottom_left, top_right, bottom_right]
        

fig = plt.figure()
fig.set_dpi(100)
fig.set_size_inches(width, height)
ax = plt.axes(xlim=(0, x_width), ylim=(0, y_height))

head = construct_tree(nodes, [[0, x_width], [0, y_height]], 0)

            
def plot_tree(cur_node: Node, ax, radius: float, lw: float, is_head: bool):
    r_val = cur_node.height / Node.max_height * .333
    g_val = cur_node.height / Node.max_height * .333
    node_visual = plt.Circle((cur_node.x, cur_node.y),
                             radius,
                             fc=(r_val,g_val,1))
    ax.add_patch(node_visual)
    
    if not is_head:
        branch = plt.Line2D((cur_node.x, cur_node.upstream.x), \
                            (cur_node.y, cur_node.upstream.y), \
                            lw = lw)
        ax.add_line(branch)

    for descendant in cur_node.downstream:
        plot_tree(descendant, ax, radius, lw, False)


for package in packages:
    package_visual = plt.Circle((package.x, package.y),
                                package_radius,
                                fc=package_color)
    ax.add_patch(package_visual)
        
plot_tree(head, ax, radius, True, .3)
plt.show()
