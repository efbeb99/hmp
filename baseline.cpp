//
// Created by 12859 on 2021/8/23.
//

#include "baseline.h"

/*
 * build cell:
 *
 * # bounds=[l_1, u_1, l_2, u_2, ..., l_d, u_d]
 *
 * recursively_build_tree(parent, cur_level, tar_level):
 * 1. if cur_level<tar_level:
 * 2.     if dim>=3:  # if dim>=3, there are dim+1 children nodes
 * 3.         b=[]
 * 4.         for d in range(dim):
 * 5.             l=parent.bounds[d*2], u=parent.bounds[d*2+1]
 * 6.             b.add(l)
 * 7.         parent.children.add(cell(b))
 * 8.     for d in range(dim): # if dim<3, there are dim children nodes
 * 9.         b=[]
 * 10.        for d2 in range(dim):
 * 11.            l=parent.bounds[d*2], u=parent.bounds[d*2+1]
 * 12.            if d==d2:
 * 13.                b.add((l+u)/2)
 * 14.                b.add(u)
 * 15.            else:
 * 16.                b.add(l)
 * 17.                b.add((l+u)/2))
 * 18.        parent.children.add(cell(b))
 * 19.for child in parent.children:
 * 20.    recursively_build_tree(child, cur_level+1, tar_level)
 *
 * cell::get_vertexes():
 * # there are dim vertexes
 * 1. for d in range(dim):
 * 2.     v=[]
 * 3.     for d2 in range(dim):
 * 4.         if d==d2:
 * 5.             v.add(this.bounds[d*2+1])  # add upper bound
 * 6.         else:
 * 7.             v.add(this.bounds[d*2])    # add lower bound
 * 8.     this.vertexes.add(v)
 */


/*
 * baseline:
 *     for each cell, calculate r-kskyband
 *
 */


// the height is no more than 10


/*
 * CSA:
 * # pre build the quad-tree and don't count for time
 * # each cell maintains a "dominated" count array
 * CSA():
 * 1. kskyband, BBS (option dataset P_0-->P)
 * 2. for i in range(len(P)):       # r-dominate test and count r-dominate
 * 3.     for j in range(i+1, len(P)):
 * 4.         root.CSA_insert(i, j, P)
 * 5. for left_c in all_leaf_cell:  # update RKS
 * 6.     for i in range(len(P)):
 * 7.         if left_c.dmc[i]<k:
 * 8.             left_c.rkskyband.add(i)
 *
 * cell::CSA_insert(i, j, P):
 * 1. if this.dmc[i]>=k or this.dmc[j]>=k:
 * 2.     return
 * 3. if this.rdominate(P[i], P[j]):
 * 4.     this.recursively_update_dmc(j)
 * 5. elif this.rdominate(P[j], P[i]):
 * 6.     this.recursively_update_dmc(i)
 * 7. else:
 * 8.     for child in this.children:
 * 9.         child.CSA_insert(i, j, P)
 *
 * cell::recursively_update_dmc(i):
 * 1. if this.dmc[i]<k:
 * 2.     this.dmc[i]+=1
 * 3.     for child in this.children:
 * 4.         child.recursively_update_dmc(i)
 */

/*
 * CSA+
 * # each cell add a r-dominate graph
 * # be careful such that dmc is a "dominated" count array
 * # but rdo_graph is a "dominate" graph
 * cell::CSAp_insert(i, j, p):
 * 1. if this.dmc[i]>=k or this.dmc[j]>=k:
 * 2.     return
 * 3. if this.rdo_graph[i].find(j)==True or this.rdo_graph[j].find(i)==True: # a pruning
 * 4.     return
 * 5. if this.rdominate(P[i], P[j]):
 * 6.     this.CSAp_update(i, j)
 * 7. elif this.rdominate(P[j], P[i]):
 * 8.     this.CSAp_update(j, i)
 * 9. else:
 * 10.    for child in this.children:
 * 11.        child.insert(i, j, P)
 *
 * cell::CSAp_update(i, j):
 * 1. this.recursively_update_dmc(j)
 * 2. this.recursively_update_graph(i, j)
 * 3. for id in this.rdo_graph[j]:
 * 4.     if this.rdo_graph[i].find(id)==False: # a new option
 * 5.         this.recursively_update_graph(i, id)
 * 6.         this.recursively_update_dmc(id)
 *
 * cell::recursively_update_graph(i, id):
 * 1. if this.dmc[id]>=k or this.rdo_graph[i].find(id)==True:
 * 2.     return
 * 3. this.rdo_graph[i].add(id)
 * 4. for child in this.children:
 * 5.     child.recursively_update_graph(i, id)
 */

/*
 * MDA
 * # for an option p in cell c, UB and LB only calculate for once
 * # insert option by option
 * # maintain a super set of RKS rather than exact RKS
 *
 * # first calculate UB(p)
 * # check relationship between UB(p) and k-th largest LB(p_i)
 * # prune p or calculate LB(p)
 * # after insert all options, tighten superset of RKS with UB(p) and current k-th largest LB(p_i)
 * #
 * # I prefer OPTION 1
 *
 * MDA():
 * 1. for id in kskyband:
 * 2.     root.MDA_insert(id)
 * 3. for leaf in all_leaves:
 * 4.     leaf.superSet2RKS()   # OPTION 1 or OPTION 2
 *
 * cell::MDA_insert(id)
 * 1. theta=this.heap.top()  # a min heap with size k and initialized with 0
 * 2. lb, ub=this.bound(id)  # finding minimal and maximal share the same execution
 * 3. if ub>theta:          # may be one of r-k-skyband
 * 4.     if lb>=theta:
 * 5.         this.heap.push(lb)
 * 6.         this.heap.pop()
 * 7.     if this.isLeaf():
 * 8.         this.s_rkskyband.add([id, lb, ub])
 * 9.     else:
 * 10.        for child in this.children:
 * 11.            child.MDA_insert(id)
 */