#include "model.h"
#include <QtMath>
#include <QHash>
#include "grid.h"

struct sModelPrivateData
{
	ModelNode *rootNode = nullptr;		// ���ڵ�
	QHash<QString, ModelNode*> nodeMap; // �ڵ�����ӳ��
	QOpenGLShaderProgram shader;
	QMatrix4x4 view;
	QMatrix4x4 projection;
	QMatrix4x4 model;
	QVector3D cameraOffset;
	QVector2D cameraPan;
	float cameraDistance = 5.0f;
	// ģ�ͱ߽��
	QVector3D minBounds;
	QVector3D maxBounds;
	cGrid grid;
};

cModel::cModel()
{
	d = new sModelPrivateData;
}

cModel::~cModel()
{
	if (d->rootNode)
		delete d->rootNode;
	d->nodeMap.clear();
	delete d;
}

void cModel::init()
{
	if (!d->shader.isLinked())
	{
		d->shader.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Resources/model.vs");
		d->shader.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/Resources/model.fs");
		if (!d->shader.link())
			qDebug() << "cModel Shader Link Failed!";
		else
			qDebug() << "cModel Shader Link Success!";
	}
	d->grid.init();
}

void cModel::draw()
{
	d->shader.bind();

	d->view.setToIdentity();
	d->view.translate(d->cameraPan.x(), d->cameraPan.y(), -d->cameraDistance);
	d->view.lookAt(QVector3D(0, 0, d->cameraDistance), QVector3D(0, 0, -1), QVector3D(0, 1, 0));

	d->shader.setUniformValue("view", d->view);
	d->shader.setUniformValue("projection", d->projection);

	d->rootNode->updateTransform(QMatrix4x4());
	renderNode(d->rootNode);

	QMatrix4x4 mvp = d->projection * d->view * d->rootNode->globalTransform;
	d->grid.updateMvp(mvp);
	d->grid.draw();

	d->shader.release();
}

void cModel::renderNode(ModelNode * node)
{
	d->shader.setUniformValue("model", node->globalTransform);

	// ��Ⱦ�ڵ���������
	for (auto mesh : node->meshes)
		mesh->drawGl(d->shader);

	// �ݹ���Ⱦ�ӽڵ�
	for (auto child : node->children)
		renderNode(child);
}

void cModel::loadModel(QString path)
{
	if (d->rootNode)
		delete d->rootNode;
	d->nodeMap.clear();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		path.toStdString(),
		aiProcess_Triangulate |
		aiProcess_GenNormals |
		aiProcess_FlipUVs
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		throw std::runtime_error("Assimp����ʧ��: " + std::string(importer.GetErrorString()));


	d->minBounds = QVector3D(FLT_MAX, FLT_MAX, FLT_MAX);
	d->maxBounds = QVector3D(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	// ����ģ��
	d->rootNode = processNode(scene->mRootNode, scene);

	// ����ģ�;��󣨾��� + ���ţ�
	QVector3D center = (d->minBounds + d->maxBounds) * 0.5f;
	QVector3D size = d->maxBounds - d->minBounds;
	float maxSize = qMax(size.x(), qMax(size.y(), size.z()));
	float scaleFactor = 5.0f / maxSize;

	// �����ͶӰͨ��ʹ�����½Ƕȣ�
	// ��Y����ת45��
	// ��X����ת35.264�ȣ�arctan(1/��2)��
	//d->rootNode->localTransform.rotate(35.264f, 1.0f, 0.0f, 0.0f);
	d->rootNode->localTransform.rotate(-45.0f, 0.0f, 1.0f, 0.0f);
	d->rootNode->localTransform.scale(scaleFactor);
	d->rootNode->localTransform.translate(-center);
	d->rootNode->markDirty();

	// �����ʼ�������
	d->cameraDistance = (maxSize * scaleFactor * 0.5f) / tan(qDegreesToRadians(45.0f / 2.0f));
}

void cModel::translate(const float & dx, const float & dy)
{
	// ֱ�Ӱ���ĻXY�����ƶ������
	float sensitivity = 0.5f * d->cameraDistance;
	d->cameraPan += QVector2D(dx * sensitivity, dy * sensitivity);
	// �����Ҫ����ȫ�ֱ任
	d->rootNode->markDirty();

	/*** �������ַ�����ʹģ�Ͱ��������XY�������ƽ��
	// ����NDC�ռ�ƽ������
	QVector3D translation(dx, dy, 0.0f);

	// ת��������ռ�
	QMatrix4x4 invProjView = (d->projectionMatrix * d->viewMatrix).inverted();
	translation = invProjView.mapVector(translation);

	d->modelMatrix.translate(translation);
	***/
}

void cModel::rotate(const float & dx, const float & dy)
{
	// ������Y����ת
	QMatrix4x4 rotY;
	rotY.rotate(dx * 0.5f, QVector3D(0, 1, 0));
	d->rootNode->localTransform = rotY * d->rootNode->localTransform;

	// ������X����ת
	QMatrix4x4 rotX;
	rotX.rotate(dy * 0.5f, QVector3D(1, 0, 0));
	d->rootNode->localTransform = rotX * d->rootNode->localTransform;

	// �����Ҫ����ȫ�ֱ任
	d->rootNode->markDirty();
}

void cModel::scale(const float & delta)
{
	d->rootNode->localTransform.scale(1.0f + delta);
	d->rootNode->markDirty();
}

void cModel::resize(int w, int h)
{
	d->projection.setToIdentity();
	d->projection.perspective(45.0f, GLfloat(w) / h, 0.1f, 1000.0f);
}

void cModel::translateNode(const QString & nodeName, const eTranslateDirection & direction, float value)
{
	switch (direction)
	{
	case eTranslateDirection::GO_X:
	{
		translateNodeByName(nodeName, QVector3D(-value, 0.0f, 0.0f));
	}break;
	case eTranslateDirection::GO_Y:
	{
		translateNodeByName(nodeName, QVector3D(0.0f, 0.0f, -value));
	}break;
	case eTranslateDirection::GO_Z:
	{
		translateNodeByName(nodeName, QVector3D(0.0f, value, 0.0f));
	}break;
	default:
		break;
	}
}

ModelNode * cModel::processNode(aiNode * node, const aiScene * scene)
{
	// ����һ���½ڵ�
	ModelNode* newNode = new ModelNode;
	newNode->name = node->mName.C_Str();

	// ����ڵ㵽ӳ���
	d->nodeMap[newNode->name] = newNode;

	// ����ڵ����������
	for (unsigned i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		newNode->meshes.push_back(processMesh(mesh, scene));
	}

	// �ݹ鴦���ӽڵ�
	for (unsigned i = 0; i < node->mNumChildren; i++)
	{
		ModelNode* child = processNode(node->mChildren[i], scene);
		newNode->children.push_back(child);
	}

	return newNode;
}

cMesh * cModel::processMesh(aiMesh * mesh, const aiScene * scene)
{
	QVector<Vertex> vertices;
	QVector<unsigned int> indices;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.Position.setX(mesh->mVertices[i].x);
		vertex.Position.setY(mesh->mVertices[i].y);
		vertex.Position.setZ(mesh->mVertices[i].z);
		if (mesh->HasNormals())
		{
			vertex.Normal.setX(mesh->mNormals[i].x);
			vertex.Normal.setY(mesh->mNormals[i].y);
			vertex.Normal.setZ(mesh->mNormals[i].z);
		}
		if (mesh->mTextureCoords[0])
		{
			vertex.TexCoords.setX(mesh->mTextureCoords[0][i].x);
			vertex.TexCoords.setY(mesh->mTextureCoords[0][i].y);
		}
		else
			vertex.TexCoords = QVector2D(0.0f, 0.0f);

		// ���°�Χ��
		d->minBounds.setX(qMin(d->minBounds.x(), mesh->mVertices[i].x));
		d->minBounds.setY(qMin(d->minBounds.y(), mesh->mVertices[i].y));
		d->minBounds.setZ(qMin(d->minBounds.z(), mesh->mVertices[i].z));
		d->maxBounds.setX(qMax(d->maxBounds.x(), mesh->mVertices[i].x));
		d->maxBounds.setY(qMax(d->maxBounds.y(), mesh->mVertices[i].y));
		d->maxBounds.setZ(qMax(d->maxBounds.z(), mesh->mVertices[i].z));

		vertices.push_back(vertex);
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	Material mat;
	aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
	//aiColor3D ambientColor(0.f, 0.f, 0.f);
	//aiMat->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
	//mat.ambient = QVector3D(ambientColor.r, ambientColor.g, ambientColor.b);

	aiColor3D diffuseColor(0.f, 0.f, 0.f);
	aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
	mat.diffuse = QVector3D(diffuseColor.r, diffuseColor.g, diffuseColor.b);

	aiColor3D specularColor(0.f, 0.f, 0.f);
	aiMat->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
	mat.specular = QVector3D(specularColor.r, specularColor.g, specularColor.b);

	// ��ȡ����ȣ�����ȣ�
	float shininess = 32.0f;
	aiMat->Get(AI_MATKEY_SHININESS, shininess);
	mat.shininess = shininess;

	return new cMesh(vertices, indices, mat);
}

void cModel::translateNodeByName(const QString & nodeName, QVector3D offset)
{
	auto it = d->nodeMap.find(nodeName);
	if (it != d->nodeMap.end())
	{
		it.value()->localTransform.translate(offset);
		it.value()->markDirty();
	}
}

void cModel::rotateNodeByName(const QString & nodeName, float angleDegrees, QVector3D axis)
{
	auto it = d->nodeMap.find(nodeName);
	if (it != d->nodeMap.end())
	{
		QMatrix4x4 rotation;
		rotation.rotate(angleDegrees, axis);
		it.value()->localTransform = rotation * it.value()->localTransform;
		it.value()->markDirty();
	}
}
