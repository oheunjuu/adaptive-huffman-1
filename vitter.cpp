/**
 * Adaptive Huffman
 *
 * Author: Djuned Fernando Djusdek
 *         5112.100.071
 *         Informatics - ITS
 */

#include <iostream>
#include <algorithm>
#include <vector>
#include <cstring>

#define SYMBOL 256
#define NUMBER 512

typedef struct node{
	unsigned char symbol; // symbol, default string kosong
	int weight,           // bobot
	    number;           // nomor urut
	node *parent,         // orang tua
	     *left,           // anak kiri
	     *right;          // anak kanan
} node;

typedef std::pair<int, node*> my_pair;

void create_node(node **leaf, unsigned char symbol, bool is_nyt) {
	node *temp =  (node*) malloc(sizeof(node));
	if (is_nyt) {
		temp->symbol = 0x00;
		temp->weight = 0;
	} else {
		temp->symbol = symbol;
		temp->weight = 1;	
	}
	temp->parent = NULL;
	temp->left = NULL;
	temp->right = NULL;
		
	*leaf = temp;
	
	return;
}

void insert_node(node **root, node *leaf) {
	if (*root == NULL) {
		*root = leaf;
	} else {
		node *mass = *root;
		node *temp = *root;
		node *temp2 = *root;
		do {
			while(temp->left != NULL) {
				temp2 = temp;
				temp = temp->left;
			}
			if (temp2->left->weight == 0) {
				temp2->left = leaf;
				temp2->left->parent = temp2;
				break;
			} else {
				temp = mass->right;
				temp2 = mass->right;
				mass = mass->right;
			}
		} while(1);
	}
	return;
}

void merge_node(node **tree, node *left, node *right) {
	node *temp = (node*) malloc(sizeof(node));
	temp->weight = left->weight + right->weight;
	temp->left = left;
	temp->right = right;
	temp->left->parent = temp;
	temp->right->parent = temp;
	temp->symbol = 0x00;
	*tree = temp;
	
	return;
}

void search_higher_block(node **tree, int weight, int *number, int parent_number, node **position, char *l_r) {
	if ((*tree)->weight == weight && (*tree)->number > *number && (*tree)->number != parent_number) {
		*position = (*tree)->parent;
		*number = (*tree)->number;
		if ((*tree)->parent->left->number == (*tree)->number) {
			strcpy(l_r, "left");
		} else {
			strcpy(l_r, "right");
		}
	}
	
	if ((*tree)->left != NULL)
		search_higher_block(&(*tree)->left, weight, &*number, parent_number, &*position, l_r);
	
	if ((*tree)->right != NULL)
		search_higher_block(&(*tree)->right, weight, &*number, parent_number, &*position, l_r);
		
	return;
}

void switch_node(node *tree, char *l_r, node *sibling, char *l_r_sibling) {	
	if (strcmp(l_r, "left") == 0 && strcmp(l_r_sibling, "left") == 0) {
		node *temp = tree->left;
		tree->left = sibling->left;
		sibling->left = temp;
		
		tree->left->parent = tree;
		sibling->left->parent = sibling;
		
	} else if (strcmp(l_r, "left") == 0) {
		node *temp = tree->left;
		tree->left = sibling->right;
		sibling->right = temp;
		
		tree->left->parent = tree;
		sibling->right->parent = sibling;
		
	} else if (strcmp(l_r_sibling, "left") == 0) {
		node *temp = tree->right;
		tree->right = sibling->left;
		sibling->left = temp;
		
		tree->right->parent = tree;
		sibling->left->parent = sibling;
		
	} else {
		node *temp = tree->right;
		tree->right = sibling->right;
		sibling->right = temp;
		
		tree->right->parent = tree;
		sibling->right->parent = sibling;
		
	}

	return;
}

void queueing_node(node **tree, std::vector<my_pair> *queue, int deep) {
	(*queue).push_back(std::make_pair(deep, *tree));
	
	if ((*tree)->right != NULL) {
		queueing_node(&(*tree)->right, &*queue, deep+1);
	}
	
	if ((*tree)->left != NULL) {
		queueing_node(&(*tree)->left, &*queue, deep+1);
	}
	
	return;
}

void increment_weight(node **tree) {
	if ((*tree)->left != NULL && (*tree)->right != NULL)
		(*tree)->weight = (*tree)->left->weight + (*tree)->right->weight;
	else
		(*tree)->weight++;
	
	return;
}

void find_external_symbol(node **tree, unsigned char symbol, node **position) {
	if ((*tree)->left != NULL)
		find_external_symbol(&(*tree)->left, symbol, &*position);
		
	if ((*tree)->symbol == symbol) {
		*position = *tree;
	}
	
	if ((*tree)->right != NULL)
		find_external_symbol(&(*tree)->right, symbol, &*position);
		
	return;
}

void print_tree(node **tree, int deep) {
	if((*tree)->left != NULL)
		print_tree(&(*tree)->left, deep+1);
	
	if((*tree)->right != NULL)
		print_tree(&(*tree)->right, deep+1);
	
	printf("%d 0x%2x %c %d %d\n", deep, (*tree)->symbol, (*tree)->symbol, (*tree)->number, (*tree)->weight);
	
	return;
}

bool my_sort(my_pair p, my_pair q) {
	return p.first < q.first;
}

void update(node **tree, unsigned char symbol, std::vector<unsigned char> *dictionary) {
	// search in dictionary
	std::vector<unsigned char>::iterator it;
	it = std::search_n ((*dictionary).begin(), (*dictionary).end(), 1, symbol);
	
	node *temp;
	
	// NYT
	if (it != (*dictionary).end()) {
		find_external_symbol(&*tree, symbol, &temp);
		
		node *inner_temp = NULL;
		char l_r[10];
		if (temp->parent->left->number == temp->number) {
			strcpy(l_r, "left");
		} else {
			strcpy(l_r, "right");
		}
		
		char l_r_sibling[10];
		int number = temp->number;
		search_higher_block(&*tree, temp->weight, &number, temp->parent->number, &inner_temp, l_r_sibling);
		if (inner_temp != NULL) {
			switch_node(temp->parent, l_r, inner_temp, l_r_sibling);
		}
				
	} else {
		node *new_nyt;
		create_node(&new_nyt, 0x00, true);
		node *new_node;
		create_node(&new_node, symbol, false);

		node *old_nyt = NULL;
		merge_node(&old_nyt, new_nyt, new_node);
		
		insert_node(&*tree, old_nyt);
		
		// goto old nyt
		temp = old_nyt;
		
		// give number
		std::vector<my_pair> queue;
		queueing_node(&*tree, &queue, 0);
		std::sort(queue.begin(), queue.end(), my_sort);
		
		int num = NUMBER;
		for (int i=0; i<queue.size(); i++) {
			queue.at(i).second->number = num--;
//			std::cout << queue.at(i).second->symbol << " " << queue.at(i).second->number << '\n';
		}
		
		(*dictionary).push_back(symbol);

	}
	
	// increment weight
	increment_weight(&temp);
	
	// for checking tree
	print_tree(&*tree, 0);
	std::cout << '\n';
	
	while(temp->parent != NULL) {
		// go to parent node
		temp = temp->parent;
		
		// if not root
		if (temp->parent != NULL)
		{
			node *inner_temp = NULL;
			
			char l_r[10];
			if (temp->parent->left->number == temp->number) {
				strcpy(l_r, "left");
			} else {
				strcpy(l_r, "right");
			}
			
			char l_r_sibling[10];
			int number = temp->number;
			search_higher_block(&*tree, temp->weight, &number, temp->parent->number, &inner_temp, l_r_sibling);
			if (inner_temp != NULL) {
				switch_node(temp->parent, l_r, inner_temp, l_r_sibling);
			}
			
		}
		increment_weight(&temp);
		
		std::vector<my_pair> queue;
		queueing_node(&*tree, &queue, 0);
		
		std::sort(queue.begin(), queue.end(), my_sort);
		
		int num = NUMBER;
		for (int i=0; i<queue.size(); i++) {
			queue.at(i).second->number = num--;
		}
		
		// for checkint tree
		print_tree(&*tree, 0);
		std::cout << '\n';
	}
	
	*tree = temp;
	
	return;
}

int main() {
	node *root;
	root = NULL;
	node *nyt;
	create_node(&nyt, 0x00, true);

	std::vector<unsigned char> dictionary;
	
	update(&root, (unsigned char)'a', &dictionary);
	
	update(&root, (unsigned char)'a', &dictionary);

	update(&root, (unsigned char)'r', &dictionary);
	
	update(&root, (unsigned char)'d', &dictionary);
	
	update(&root, (unsigned char)'v', &dictionary);

	update(&root, (unsigned char)'j', &dictionary);

	update(&root, (unsigned char)'j', &dictionary);

//	update(&root, (unsigned char)'j', &dictionary);
//
//	update(&root, (unsigned char)'j', &dictionary);
	
//	update(&root, (unsigned char)'a', &dictionary);
					
	print_tree(&root, 0);
	
	return 0;
}
