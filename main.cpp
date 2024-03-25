#include <QtCore/QDebug>
#include <QtGui/QMatrix4x4>
#include <QtGui/QVector3D>
#include <QtGui/QOpenGLFunctions>
#include <QtOpenGL/QOpenGLBuffer>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

class OpenGLWidget : public QOpenGLWidget, private QOpenGLFunctions
{
public:
    OpenGLWidget()
    {
        setWindowTitle("OpenGL ES 2.0, Qt6, C++");
        resize(400, 400);
    }

private:
    QOpenGLBuffer m_vertPosBuffer;
    QOpenGLShaderProgram m_program;
    int m_numVertices;
    QMatrix4x4 m_modelMatrix;

    void initializeGL() override
    {
        initializeOpenGLFunctions();
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glEnable(GL_DEPTH_TEST);

        Assimp::Importer importer;
        const char *path = "assets/models/plane-blender.dae";
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            qDebug() << "Assimp Error:" << importer.GetErrorString();
            QMessageBox::critical(this, "Assimp Error:", importer.GetErrorString());
            return;
        }

        m_numVertices = scene->mMeshes[0]->mNumVertices;
        float vertPositions[m_numVertices * 3];
        int vertPosIndex = 0;
        for (int i = 0; i < m_numVertices; i++)
        {
            vertPositions[vertPosIndex++] = scene->mMeshes[0]->mVertices[i].x;
            vertPositions[vertPosIndex++] = scene->mMeshes[0]->mVertices[i].y;
            vertPositions[vertPosIndex++] = scene->mMeshes[0]->mVertices[i].z;
           // qDebug() << scene->mMeshes[0]->mVertices[i].x << ", "
           //          << scene->mMeshes[0]->mVertices[i].y << ", "
           //           << scene->mMeshes[0]->mVertices[i].z;
           //  qDebug() << "\n";
        }
        m_vertPosBuffer.create();
        m_vertPosBuffer.bind();
        m_vertPosBuffer.allocate(vertPositions, sizeof(vertPositions));

        const char *vertShaderSrc =
            "attribute vec3 aPosition;"
            "uniform mat4 uModelMatrix;"
            "void main()"
            "{"
            "    gl_Position = uModelMatrix * vec4(aPosition, 1.0);"
            "}";
        const char *fragShaderSrc =
            "void main()"
            "{"
            "    gl_FragColor = vec4(0.5, 0.2, 0.7, 1.0);"
            "}";
        m_program.create();
        m_program.addShaderFromSourceCode(QOpenGLShader::ShaderTypeBit::Vertex,
            vertShaderSrc);
        m_program.addShaderFromSourceCode(QOpenGLShader::ShaderTypeBit::Fragment,
            fragShaderSrc);
        m_program.link();
    }

    void resizeGL(int w, int h) override
    {
        glViewport(0, 0, w, h);
    }

    void paintGL() override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_modelMatrix.setToIdentity();
        m_modelMatrix.scale(0.5);
        m_program.bind();
        m_program.setUniformValue("uModelMatrix", m_modelMatrix);
        m_vertPosBuffer.bind();
        m_program.setAttributeBuffer("aPosition", GL_FLOAT, 0, 3);
        m_program.enableAttributeArray("aPosition");
        glDrawArrays(GL_TRIANGLES, 0, m_numVertices);
    }
};

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::ApplicationAttribute::AA_UseDesktopOpenGL);
    QApplication app(argc, argv);
    OpenGLWidget w;
    w.show();
    return app.exec();
}
