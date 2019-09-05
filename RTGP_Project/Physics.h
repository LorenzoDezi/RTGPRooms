#pragma once
/*
Physics class - v1:
- initialization of the physics simulation using the Bullet librarty

The class sets up the collision manager and the resolver of the constraints, using basic general-purposes methods provided by the library. Advanced and multithread methods are available, please consult Bullet documentation and examples

createRigidBody method sets up a  Box or Sphere Collision Shape. For other Shapes, you must extend the method.

author: Davide Gadia

Real-Time Graphics Programming - a.a. 2018/2019
Master degree in Computer Science
Universita' degli Studi di Milano
*/

#include <bullet/btBulletDynamicsCommon.h>
#include "bullet/BulletCollision/CollisionShapes/btShapeHull.h"
#include <bullet/btBulletCollisionCommon.h>
#include "Model.h"
#include <glm/glm.hpp>

//enum to identify the 2 considered Collision Shapes
enum shapes { BOX, SPHERE, CAPSULE };

///////////////////  Physics class ///////////////////////
class Physics
{
public:

	btDiscreteDynamicsWorld * dynamicsWorld; // the main physical simulation class
	btAlignedObjectArray<btCollisionShape*> collisionShapes; // a vector for all the Collision Shapes of the scene
	btDefaultCollisionConfiguration* collisionConfiguration; // setup for the collision manager
	btCollisionDispatcher* dispatcher; // collision manager
	btBroadphaseInterface* overlappingPairCache; // method for the broadphase collision detection
	btSequentialImpulseConstraintSolver* solver; // constraints solver

	//////////////////////////////////////////
	// constructor
	// we set all the classes needed for the physical simulation
	Physics();

	//////////////////////////////////////////
	// Method for the creation of a rigid body, based on a Box or Sphere Collision Shape
	// The Collision Shape is a reference solid that approximates the shape of the actual object of the scene. The Physical simulation is applied to these solids, and the rotations and positions of these solids are used on the real models.
	btRigidBody* createRigidBody(int type, glm::vec3 pos, glm::vec3 size,
		glm::vec3 rot, float m, float friction, float restitution, float angularDamping = INFINITY, float rollingFriction = INFINITY);

	void createStaticRigidBodyWithTriangleMesh(Model &model, glm::vec3 pos, glm::vec3 size, glm::vec3 rot, float friction = 0.0f);

	//////////////////////////////////////////
	// We delete the data of the physical simulation when the program ends
	void Clear();

};
