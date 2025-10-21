#include "AgentDemo.h"

#if 0

EXAMPLE B: javascript-AST

1. Introduction

Here’s a toy example for JavaScript Abstract Syntax Trees (AST) that shows the creation, insertion, modification, and deletion of nodes.

Atoms (predicates)
	• has_node(Node)
	• has_children(Node)
	• has_parent(Node, Parent)
	• has_type(Node, Type) (Type∈{FunctionDeclaration, VariableDeclaration, BinaryExpression, AssignmentExpression})
	• has_name(Node, Name)
	• has_value(Node, Value)
	• is_assigned(Node)
	• is_called(Node)
	• is_argument(Node)
	• is_declaration(Node)
	• is_expression(Node)
	• is_condition(Node)

2. Actions

CreateNode(Type, Parent)
Pre: type∈{FunctionDeclaration, VariableDeclaration, BinaryExpression, AssignmentExpression}, has_node(Parent)
Post: has_node(Node), has_children(Node), has_parent(Node,Parent), has_type(Node,Type)

InsertChild(Parent, Child)
Pre: has_node(Parent), has_node(Child), has_parent(Child, None)
Post: has_children(Parent), has_parent(Child, Parent)

ModifyNode(Node, Name, Value)
Pre: has_node(Node), (has_type(Node, FunctionDeclaration) or has_type(Node, VariableDeclaration))
Post: has_name(Node, Name), has_value(Node, Value)

DeleteNode(Node)
Pre: has_node(Node)
Post: ¬has_node(Node)

3. Initial WorldState

has_node(program)
has_children(program)
has_type(program, None)

4. Goal

has_node(program)
has_children(program)
is_declaration(node1)
is_declaration(node2)
is_expression(node1)
is_condition(node3)

5. Result

List of actions:
1. CreateNode(FunctionDeclaration, program)
2. CreateNode(VariableDeclaration, program)
3. InsertChild(program, node1)
4. InsertChild(program, node2)
5. InsertChild(node1, node3)
6. ModifyNode(node1, "el", "hello")
7. DeleteNode(node2)

#endif




#if 0


EXAMPLE B: javascript-AST visitor-agent

1. Introduction

The agent moves in the AST tree. It can move towards the root, sideways from the parent, and towards a leaf. It can also perform actions in the nodes it visits. 

Actions 
• goToParent(Node) 
Pre: has_parent(Node, Parent)
Post: has_node(Parent)

• goToChild(Node, Child)
Pre: has_node(Node), has_parent(Child, Node)
Post: has_node(Child)

• goToNextSibling(Node, Sibling)
Pre: has_node(Node), has_node(Sibling), has_parent(Node, Parent), has_parent(Sibling, Parent)
Post: has_node(Sibling)

• performAction(Node, Action)
Pre: has_node(Node), has_action(Action)
Post: ¬has_node(Node)


2. Initial WorldState

has_node(program)
has_children(program)
has_type(program, None)

3. Goal

has_node(node1)
is_action(Action1)
is_action(Action2)

4. Result

List of actions:
1. goToChild(program, node1)
2. performAction(node1, Action1)
3. goToNextSibling(node1, node2)
4. performAction(node2, Action2)

#endif
