#include <GL/glew.h>
#include <glm/glm.hpp>
class SceneNode {
public:
	glm::mat4 m;
	void(*f)();
	SceneNode *sibling;
	SceneNode *root;
	SceneNode *child;

	SceneNode();
	SceneNode *getSibling();
	SceneNode *getRoot();
	SceneNode *getChild();

	void setSibling(SceneNode *sibling);
	void setRoot(SceneNode *root);
	void setChild(SceneNode *child);
};
