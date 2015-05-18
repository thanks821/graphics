// #include <GL/glew.h>
#include <QtWidgets>
#include <QtOpenGL>
#include <QVector3D>
#include "Viewer.hpp"
#include <iostream>
#include <math.h>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

Viewer::Viewer(const QGLFormat& format, QWidget *parent)
    : QGLWidget(format, parent)
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    , mVertexBufferObject(QOpenGLBuffer::VertexBuffer)
    , mVertexArrayObject(this)
    #else
    , mVertexBufferObject(QGLBuffer::VertexBuffer)
    #endif
{
    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(update()));
    mTimer->start(500/30);


    m_game = new Game(10,20);
    gameTimer = new QTimer(this);
    connect(gameTimer, SIGNAL(timeout()), this, SLOT(gameTick()));
    cout<<"Game speed: "<<( (5000 - (80 * gameSpeed * 5) )/ 30) <<endl;
    gameTimer->start( (5000 - (80 * gameSpeed * 5 ) )/ 30);
    rotateTimer = new QTimer(this);
    connect(rotateTimer,SIGNAL(timeout()),this,SLOT(trackSpeed()));


}

Viewer::~Viewer() {

}

QSize Viewer::minimumSizeHint() const {
    return QSize(50, 50);
}

QSize Viewer::sizeHint() const {
    return QSize(300, 600);
}

void Viewer::initializeGL() {
    QGLFormat glFormat = QGLWidget::format();
    if (!glFormat.sampleBuffers()) {
        std::cerr << "Could not enable sample buffers." << std::endl;
        return;
    }

    glClearColor(0.7, 0.7, 1.0, 0.0);

    if (!mProgram.addShaderFromSourceFile(QGLShader::Vertex, "shader.vert")) {
        std::cerr << "Cannot load vertex shader." << std::endl;
        return;
    }

    if (!mProgram.addShaderFromSourceFile(QGLShader::Fragment, "shader.frag")) {
        std::cerr << "Cannot load fragment shader." << std::endl;
        return;
    }

    if ( !mProgram.link() ) {
        std::cerr << "Cannot link shaders." << std::endl;
        return;
    }

    float triangleData[] = {
        //  X     Y     Z
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
    };



#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    mVertexArrayObject.create();
    mVertexArrayObject.bind();

    mVertexBufferObject.create();
    mVertexBufferObject.setUsagePattern(QOpenGLBuffer::StaticDraw);
#else

    /*
     * if qt version is less than 5.1, use the following commented code
     * instead of QOpenGLVertexVufferObject. Also use QGLBuffer instead of
     * QOpenGLBuffer.
     */
    uint vao;

    typedef void (APIENTRY *_glGenVertexArrays) (GLsizei, GLuint*);
    typedef void (APIENTRY *_glBindVertexArray) (GLuint);

    _glGenVertexArrays glGenVertexArrays;
    _glBindVertexArray glBindVertexArray;

    glGenVertexArrays = (_glGenVertexArrays) QGLWidget::context()->getProcAddress("glGenVertexArrays");
    glBindVertexArray = (_glBindVertexArray) QGLWidget::context()->getProcAddress("glBindVertexArray");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    mVertexBufferObject.create();
    mVertexBufferObject.setUsagePattern(QGLBuffer::StaticDraw);
#endif

    if (!mVertexBufferObject.bind()) {
        std::cerr << "could not bind vertex buffer to the context." << std::endl;
        return;
    }

    mVertexBufferObject.allocate(triangleData, 3 * 3 * sizeof(float));

    mProgram.bind();

    mProgram.enableAttributeArray("vert");
    mProgram.setAttributeBuffer("vert", GL_FLOAT, 0, 3);

    // mPerspMatrixLocation = mProgram.uniformLocation("cameraMatrix");
    mMvpMatrixLocation = mProgram.uniformLocation("mvpMatrix");
    colorLocation = mProgram.uniformLocation("frag_color");

    QColor color(0, 255, 0, 255);

    mProgram.setUniformValue(colorLocation, color);


}

void Viewer::paintGL() {
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 1, 0))
    mVertexArrayObject.bind();
#endif

    makeWell();
    for(int y = 0 ; y < m_game->getHeight() + 4; y ++){
        for(int x = 0; x < m_game->getWidth()  ; x ++) {
            int blockID = m_game->get(y,x);
            if(m_game->get(y,x) != -1) {

                if(blockID == 0)
                    drawCubeAt(x,y,Qt::red);
                else if (blockID == 1)
                    drawCubeAt(x,y,Qt::green);
                else if (blockID == 2)
                    drawCubeAt(x,y,Qt::blue);
                else if (blockID == 3)
                    drawCubeAt(x,y,Qt::magenta);
                else if (blockID == 4)
                    drawCubeAt(x,y,Qt::yellow);
                else if (blockID == 5)
                    drawCubeAt(x,y,Qt::darkGray);
                else if (blockID == 6)
                    drawCubeAt(x,y,Qt::cyan);
                else if (blockID == 7)
                    drawCubeAt(x,y,Qt::white);
            }
        }
    }

    if(autoRotate){
        if(lastRotateAxis == 'x'){
            rotateWorld(10,1,0,0);
        }
        else if (lastRotateAxis == 'y'){
            rotateWorld(10,0,1,0);

        }
        else if (lastRotateAxis == 'z'){
            rotateWorld(10,0,0,1);

        }
    }
}

void Viewer::resizeGL(int width, int height) {
    if (height == 0) {
        height = 1;
    }

    mPerspMatrix.setToIdentity();
    mPerspMatrix.perspective(60.0, (float) width / (float) height, 0.001, 1000);

    glViewport(0, 0, width, height);
}

void Viewer::mousePressEvent ( QMouseEvent * event ) {
    std::cerr << "Stub: button " << event->button() << " pressed\n";
    autoRotate = false;
    buttonPressed = event->button();
    prev_x = event->x();
    prev_y = event->y();
    xBeforeTick = event->x();
    yBeforeTick = event->y();
    rotateTimer->start(500);
}

void Viewer::mouseReleaseEvent ( QMouseEvent * event ) {
    //std::cerr << "Stub: button " << event->button() << " released\n";
    rotateTimer->stop();
    cout<<lastRotateAxis<<endl<<speed_x<<endl;
    if( (lastRotateAxis = 'x' && speed_x > 0) ||
            (lastRotateAxis = 'y' && speed_y > 0) ||
            (lastRotateAxis = 'z' && speed_x > 0))
    {
        autoRotate = true;
    }

}

void Viewer::mouseMoveEvent ( QMouseEvent * event ) {
    //std::cerr << "Stub: Motion at " << event->x() << ", " << event->y() << ", Button: " << buttonPressed << std::endl;
    int diffx = event->x() - prev_x;
    int diffy = event->y() - prev_y;
    int signx = (event->x() > prev_x) ? 1 : -1;
    int signy = (event->y() > prev_y) ? 1 : -1;
    //cout<<"diffx : "<< diffx <<endl;

    if(shiftState){
        if(signx == 1){
            if(diffx > 0){
                worldScale += 0.01;
                worldScale = 1.05;
            }
            else{
                worldScale -= 0.01;
                worldScale = 0.95;
            }
        }
        else{
            if(diffx < 0){
                worldScale -= 0.01;
                worldScale = 0.95;
            }
            else{
                worldScale += 0.01;
                worldScale = 1.05;
            }
        }
        cout<<"Scale :"<<worldScale<<endl;
        scaleWorld(worldScale,worldScale,worldScale);
        prev_x = event->x();
        prev_y = event->y();
        lastRotateAxis = 'a';
        return;
    }

    if(buttonPressed == 1){
        rotateWorld(signy*abs(diffy)*.5,1,0,0);
        lastRotateAxis = 'x';
        rotateSign = signy;
    }
    else if (buttonPressed == 2){
        rotateWorld(signx*abs(diffx)*.5,0,1,0);
        lastRotateAxis = 'y';
        rotateSign = signx;
    }
    else if (buttonPressed == 4){
        rotateWorld(signx*abs(diffx)*.5 ,0,0,1);
        lastRotateAxis = 'z';
        rotateSign = signx;
    }


    prev_x = event->x();
    prev_y = event->y();

}

QMatrix4x4 Viewer::getCameraMatrix() {
    QMatrix4x4 vMatrix;

    QMatrix4x4 cameraTransformation;
    QVector3D cameraPosition = cameraTransformation * QVector3D(0, 0, 20.0);
    QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);

    vMatrix.lookAt(cameraPosition, QVector3D(0, 0, 0), cameraUpDirection);

    return mPerspMatrix * vMatrix * mTransformMatrix;
}

void Viewer::translateWorld(float x, float y, float z) {
    mTransformMatrix.translate(x, y, z);
}

void Viewer::rotateWorld(float angle, float x, float y, float z) {
    mTransformMatrix.rotate(angle, x, y, z);
}

void Viewer::scaleWorld(float x, float y, float z) {
    mTransformMatrix.scale(x, y, z);
}

void Viewer::drawCubeAt(int x, int y, QColor color){


    //Initial condiditions
    for(int j = 0; j <12; j ++){
        mModelMatrices[j].setToIdentity();
        mModelMatrices[j].translate(-5 + x , -10 + y,0);
    }

    //front face
    mModelMatrices[0].translate(0,0,0);
    mModelMatrices[0].rotate(0, QVector3D(0,0,0));

    mModelMatrices[1].translate(1,1.0,0);
    mModelMatrices[1].rotate(180, QVector3D(0,0,1));

    //back face
    mModelMatrices[2].translate(0,0,-1);
    mModelMatrices[2].rotate(0, QVector3D(0,0,1));

    mModelMatrices[3].translate(1,1.0,-1);
    mModelMatrices[3].rotate(180, QVector3D(0,0,1));

    //top face


    mModelMatrices[4].translate(0,1,-1);
    mModelMatrices[4].rotate(90, QVector3D(1,0,0));


    mModelMatrices[5].translate(1,1,0);
    mModelMatrices[5].rotate(90, QVector3D(1,0,0));
    mModelMatrices[5].rotate(180, QVector3D(0,0,1));


    //bottom face
    mModelMatrices[6].translate(0,0,-1);
    mModelMatrices[6].rotate(90, QVector3D(1,0,0));


    mModelMatrices[7].translate(1,0,0);
    mModelMatrices[7].rotate(90, QVector3D(1,0,0));
    mModelMatrices[7].rotate(180, QVector3D(0,0,1));

    //left face
    mModelMatrices[8].translate(0,0,0);
    mModelMatrices[8].rotate(90, QVector3D(0,1,0));


    mModelMatrices[9].translate(0,1,-1);
    mModelMatrices[9].rotate(90, QVector3D(0,1,0));
    mModelMatrices[9].rotate(180, QVector3D(0,0,1));


    //right face
    mModelMatrices[10].translate(1,0,0);
    mModelMatrices[10].rotate(90, QVector3D(0,1,0));


    mModelMatrices[11].translate(1,1,-1);
    mModelMatrices[11].rotate(90, QVector3D(0,1,0));
    mModelMatrices[11].rotate(180, QVector3D(0,0,1));


    //draw shape
    for (int i = 0; i < 12; i++) {
        mProgram.setUniformValue(mMvpMatrixLocation, getCameraMatrix() * mModelMatrices[i]);
        mProgram.setUniformValue(colorLocation, color);
        glEnable(GL_DEPTH_TEST);
        glDrawArrays(drawMode, 0, 3);
    }



}

void Viewer::makeWell()
{
    //leftwall
    int x,y;
    for(y = m_game->getHeight()-1; y >= 0; y --){
        drawCubeAt(x-1,y,Qt::black);
        drawCubeAt( m_game->getWidth(), y,Qt::black );
    }
    drawCubeAt(x-1,y,Qt::black);
    //bottomwall
    for(x = 0; x < m_game->getWidth(); x ++){
        drawCubeAt(x,y,Qt::black);
    }
    drawCubeAt(x,y,Qt::black);
}

void Viewer::decreaseGameSpeed(){
    if (gameSpeed > 1) {
        gameSpeed--;
    }
}


void Viewer::increaseGameSpeed(){
    if (gameSpeed < 10){
        gameSpeed++;
    }
}

void Viewer::gameTick(){
    m_game->tick();
}

void Viewer::trackSpeed()
{
    speed_x = prev_x - xBeforeTick;
    speed_y = prev_y - yBeforeTick;
    xBeforeTick = prev_x;
    yBeforeTick = prev_y;

}

void Viewer::setFaceMode(){
    drawMode = GL_TRIANGLES;
}

void Viewer::setMultiMode(){

}

void Viewer::setShiftState(bool state)
{
    shiftState = state;
}

void Viewer::resetWorld()
{
    mTransformMatrix.setToIdentity();

}

void Viewer::setWireMode(){
    drawMode = GL_LINE_LOOP;


}
