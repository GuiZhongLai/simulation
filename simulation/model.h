#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "mesh.h"
#include <QMatrix4x4>
#include <QVector>

// ����ģ�ͽڵ㣨���νṹ��
struct ModelNode
{
	QMatrix4x4 localTransform;          // ���ر任����
	QMatrix4x4 globalTransform;         // ȫ�ֱ任�����Զ����㣩
	QVector<cMesh*> meshes;				// �ڵ����������
	QVector<ModelNode*> children;		// �ӽڵ��б�
	QString name;                       // �ڵ�����
	bool isDirty = true;

	// ����ȫ�ֱ任���󣨵ݹ飩
	void updateTransform(const QMatrix4x4& parentGlobalMatrix)
	{
		if (isDirty)
		{
			globalTransform = parentGlobalMatrix * localTransform;
			isDirty = false;
		}
		for (auto child : children)
			child->updateTransform(globalTransform);
	}

	void markDirty()
	{
		isDirty = true;
		for (auto child : children)
			child->markDirty();
	}

	~ModelNode()
	{
		for (auto mesh : meshes)
			delete mesh;

		for (auto child : children)
			delete child;
	}
};

enum eTranslateDirection
{
	GO_X,
	GO_Y,
	GO_Z,
};

struct sModelPrivateData;
class cModel
{
public:
	cModel();
	~cModel();

	void init();
	void draw();
	void loadModel(QString path);

public:
	void translate(const float &dx, const float &dy);
	void rotate(const float &dx, const float &dy);
	void scale(const float &delta);
	void resize(int w, int h);

	void translateNode(const QString &nodeName, const eTranslateDirection &direction, float value);

private:
	ModelNode *processNode(aiNode *node, const aiScene *scene);
	cMesh *processMesh(aiMesh *mesh, const aiScene *scene);

	void translateNodeByName(const QString &nodeName, QVector3D offset);
	void rotateNodeByName(const QString &nodeName, float angleDegrees, QVector3D axis);

	void renderNode(ModelNode *node);

private:
	sModelPrivateData *d;
};

