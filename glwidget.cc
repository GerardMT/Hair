#include "glwidget.h"

#include "collider_plane.h"
#include "collider_sphere.h"
#include "collider_triangle.h"
#include "solver_euler.h"
#include "solver_semi_implicit_euler.h"
#include "solver_verlet.h"

#include <QTimer>
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <QMouseEvent>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

GLWidget::~GLWidget()
{
    for (auto o : objects_) {
        delete o;
    }

    delete solver_euler_;
    delete solver_semi_implicit_euler_;

    delete particles_system_hair;

    delete force_field_gravity_;
    delete force_field_drag_;

    delete particle_hair_initializer_single_;
    delete particle_hair_initializer_curly_;
}


void GLWidget::initializeGL()
{    
    camera_.pos_ = glm::vec3(-1.0, 0.5, -2.0);
    camera_.fov_y_ = glm::radians(90.0f);
    camera_.lookAt(glm::vec3(0.0, 0.0, 0.0));

    Mesh *m;
    m = new Mesh();
    Mesh::ReadFromPly("../model/cube.ply", *m);

    Object *box = new Object(*m, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(1.0, 0.0, 0.0, 1.0));
    objects_.push_back(box);
    paint_gl_.push_back(box);

    solver_euler_ = new SolverEuler();
    solver_semi_implicit_euler_ = new SolverSemiImplicitEuler();
    solver_verlet_ = new SolverVerlet();

    particle_hair_initializer_single_ = new ParticleHairInitializerSingle();
    particle_hair_initializer_curly_ = new ParticleHairInitializerCurly(glm::vec3(0.0, 0.0, 0.0));

    particles_system_hair = new ParticleSystemHair(*solver_verlet_, *particle_hair_initializer_single_);
    paint_gl_.push_back(particles_system_hair);

    force_field_gravity_ = new ForceFieldGravity();
    particles_system_hair->addForceField(*force_field_gravity_);
    force_field_drag_ = new ForceFieldDrag(0.1);
    particles_system_hair->addForceField(*force_field_drag_);

    float bouncing = 0.5;
    float friction = 0.2;

    Collider *c;
    c = new ColliderPlane(glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, -1.0, 0.0), bouncing, friction);
    box->addCollider(*c);
    particles_system_hair->addCollider(*c);
    c = new ColliderPlane(glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 0.0, -1.0), bouncing, friction);
    box->addCollider(*c);
    particles_system_hair->addCollider(*c);
    c = new ColliderPlane(glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0), bouncing, friction);
    box->addCollider(*c);
    particles_system_hair->addCollider(*c);
    c = new ColliderPlane(glm::vec3(-1.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0), bouncing, friction);
    box->addCollider(*c);
    particles_system_hair->addCollider(*c);
    c = new ColliderPlane(glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0), bouncing, friction);
    box->addCollider(*c);
    particles_system_hair->addCollider(*c);

    m = new Mesh();
    Mesh::ReadFromPly("../model/sphere.ply", *m);

    glm::vec3 sphere_pos = glm::vec3(0.0f, 0.0, 0.0f);

    Object *sphere = new Object(*m, sphere_pos, glm::vec4(0.0, 0.0, 1.0, 1.0));
    objects_.push_back(sphere);
    paint_gl_.push_back(sphere);

    c = new ColliderSphere(sphere_pos, 0.5, 0.8, 0.5);
    sphere->addCollider(*c);
    particles_system_hair->addCollider(*c);

    glewInit();

    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (auto p : paint_gl_) {
        p->initialieGL();
    }

    target_frame_time_ = 1.0 / 60.0;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(target_frame_time_);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
void GLWidget::mousePressEvent(QMouseEvent *event) {
  switch (event->button()) {
  case Qt::LeftButton:
    rotate_ = true;
    rotate_last_x_ = event->x();
    rotate_last_y_ = event->y();
    break;
  }
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
void GLWidget::mouseMoveEvent(QMouseEvent *event) {
  if (rotate_) {
    int x = event->x() - rotate_last_x_;
    int y = event->y() - rotate_last_y_;
    camera_.rotate(x, y, dt_);

    rotate_last_x_ = event->x();
    rotate_last_y_ = event->y();
  }
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
  switch (event->button()) {
  case Qt::LeftButton:
    rotate_ = false;
    break;
  }
}
#pragma GCC diagnostic pop

void GLWidget::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
  case Qt::Key_W:
    forward_ = true;
    break;
  case Qt::Key_S:
    backwards_ = true;
    break;
  case Qt::Key_A:
    left_ = true;
    break;
  case Qt::Key_D:
    right_ = true;
    break;
  }
}

void GLWidget::keyReleaseEvent(QKeyEvent *event) {
  switch (event->key()) {
  case Qt::Key_W:
    forward_ = false;
    break;
  case Qt::Key_S:
    backwards_ = false;
    break;
  case Qt::Key_A:
    left_ = false;
    break;
  case Qt::Key_D:
    right_ = false;
    break;
  }
}

void GLWidget::resizeGL(int w, int h)
{
    camera_.width_ = w;
    camera_.height_ = h;

    camera_.viewport();
}

void GLWidget::paintGL() {
    chrono::steady_clock::time_point time_now = chrono::steady_clock::now();
    if (first_paint_) {
        first_paint_ = false;
        dt_ = target_frame_time_;
    }else {
        dt_ = chrono::duration_cast<chrono::nanoseconds>(time_now - time_last_).count() * 1e-9;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (forward_) {
      camera_.forward(dt_);
    }
    if (backwards_) {
      camera_.backwards(dt_);
    }
    if (left_) {
      camera_.left(dt_);
    }
    if (right_) {
      camera_.right(dt_);
    }

    camera_.compute_view_projection();

    dt_ = 0.01666666f; // TODO DEBUG

    //cout << camera_.azimuth_ << " " << camera_.inclination_ << endl;
    //cout << camera_.front_.x << " " << camera_.front_.y << " " << camera_.front_.z << endl;

    for (auto p : paint_gl_) {
        p->paintGL(dt_, camera_);
    }

    time_last_ = time_now;
}

void GLWidget::uiSolverEuler(bool v)
{
    if (v) {
        particles_system_hair->solver(*solver_euler_);
    }
}

void GLWidget::uiSolverSemiImplicitEuler(bool v)
{
    if (v) {
        particles_system_hair->solver(*solver_semi_implicit_euler_);
    }
}

void GLWidget::uiSolverVerlet(bool v)
{
    if (v) {
        particles_system_hair->solver(*solver_verlet_);
    }
}

void GLWidget::uiPaintParticles(bool v)
{
    particles_system_hair->paint_particles_ = v;
}

void GLWidget::uiPaintPath(bool v)
{
    particles_system_hair->paint_path_ = v;
}

void GLWidget::uiInitializerRope(bool v)
{
    if (v) {
        particles_system_hair->particleInitializer(*particle_hair_initializer_single_);
    }
}

void GLWidget::uiInitializerCurly(bool v)
{
    if (v) {
        particles_system_hair->particleInitializer(*particle_hair_initializer_curly_);
    }
}
