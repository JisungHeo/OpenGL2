#include <GL/glew.h>
#include <glm/glm.hpp>
#include "scene_node.hpp"

SceneNode::SceneNode() {
	this->m = glm::mat4();
	this->f = NULL;
	this->root = NULL;
	this->child = NULL;
	this->sibling = NULL;
}

SceneNode *SceneNode::getSibling() {
	return this->sibling;
}
SceneNode *SceneNode::getRoot() {
	return this->root;
}
SceneNode *SceneNode::getChild() {
	return this->child;
}

void SceneNode::setSibling(SceneNode *sibling) {
	this->sibling = sibling;
}
void SceneNode::setRoot(SceneNode *root) {
	this->root = root;
}
void SceneNode::setChild(SceneNode *child) {
	this->child = child;
}

