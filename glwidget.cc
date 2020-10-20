#include "glwidget.h"

#include "collider_plane.h"
#include "collider_sphere.h"
#include "collider_triangle.h"
#include "particle_initializer_fountain.h"
#include "particle_initializer_semi_sphere.h"
#include "particle_initializer_waterfall.h"
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

    delete particle_initializer_fountain_;
    delete particle_initializer_waterfall_;

    delete particles_system_;

    delete force_field_gravity_;
    delete force_field_drag_;
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

    particle_initializer_fountain_ = new ParticleInitializerFountain(glm::vec3(0.0, -0.9, 0.0), 5.0, 0.2);
    particle_initializer_waterfall_ = new ParticleInitializerWaterfall(glm::vec3(0.0, 0.75, 0.0), 0.5, 0.8);
    particle_initializer_semi_sphere_ = new ParticleInitializerSemiSphere(glm::vec3(0.0, 0.75, 0.0), 3.0, 0.25);

    particles_system_ = new ParticleSystem(*solver_euler_, *particle_initializer_fountain_, 500, 60.0);
    paint_gl_.push_back(particles_system_);

    force_field_gravity_ = new ForceFieldGravity();
    particles_system_->addForceField(*force_field_gravity_);
    force_field_drag_ = new ForceFieldDrag(0.1);
    particles_system_->addForceField(*force_field_drag_);

    float bouncing = 0.5;
    float friction = 0.2;

    Collider *c;
    c = new ColliderPlane(glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, -1.0, 0.0), bouncing, friction);
    box->addCollider(*c);
    particles_system_->addCollider(*c);
    c = new ColliderPlane(glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0), bouncing, friction);
    box->addCollider(*c);
    particles_system_->addCollider(*c);
    c = new ColliderPlane(glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 0.0, -1.0), bouncing, friction);
    box->addCollider(*c);
    particles_system_->addCollider(*c);
    c = new ColliderPlane(glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0), bouncing, friction);
    box->addCollider(*c);
    particles_system_->addCollider(*c);
    c = new ColliderPlane(glm::vec3(-1.0, 0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0), bouncing, friction);
    box->addCollider(*c);
    particles_system_->addCollider(*c);
    c = new ColliderPlane(glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0), bouncing, friction);
    box->addCollider(*c);
    particles_system_->addCollider(*c);

    m = new Mesh();
    Mesh::ReadFromPly("../model/triangle.ply", *m);

    Object *triangle = new Object(*m, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(0.0, 1.0, 0.0, 1.0));
    objects_.push_back(triangle);
    paint_gl_.push_back(triangle);

    c = new ColliderTriangle(glm::vec3(m->vertices_[0], m->vertices_[1], m->vertices_[2]),
                             glm::vec3(m->vertices_[3], m->vertices_[4], m->vertices_[5]),
                             glm::vec3(m->vertices_[6], m->vertices_[7], m->vertices_[8]), 1.0, 0.0);
    triangle->addCollider(*c);
    particles_system_->addCollider(*c);

    m = new Mesh();
    Mesh::ReadFromPly("../model/sphere.ply", *m);

    glm::vec3 sphere_pos = glm::vec3(0.0f, 0.5, 1.0f);

    Object *sphere = new Object(*m, sphere_pos, glm::vec4(0.0, 0.0, 1.0, 1.0));
    objects_.push_back(sphere);
    paint_gl_.push_back(sphere);

    c = new ColliderSphere(sphere_pos, 0.5, 0.8, 0.5);
    sphere->addCollider(*c);
    particles_system_->addCollider(*c);

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

    //dt_ = 0.01666666f; // TODO DEBUG

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
        particles_system_->solver(*solver_euler_);
    }
}

void GLWidget::uiSolverSemiImplicitEuler(bool v)
{
    if (v) {
        particles_system_->solver(*solver_semi_implicit_euler_);
    }
}

void GLWidget::uiSolverVerlet(bool v)
{
    if (v) {
        particles_system_->solver(*solver_verlet_);
    }
}

void GLWidget::uiParticleInitializerFountain(bool v)
{
    if (v) {
        particles_system_->particleInitializer(*particle_initializer_fountain_);
    }
}

void GLWidget::uiParticleInitializerWatterfall(bool v)
{
    if (v) {
        particles_system_->particleInitializer(*particle_initializer_waterfall_);
    }
}

void GLWidget::uiParticleInitializerSemiSphere(bool v)
{
    if (v) {
        particles_system_->particleInitializer(*particle_initializer_semi_sphere_);
    }
}

void GLWidget::uiNParticles(int n)
{
    particles_system_->nParticles(n);
}

void GLWidget::uiLifetime(double t)
{
    particles_system_->life_time_ = t;
}
