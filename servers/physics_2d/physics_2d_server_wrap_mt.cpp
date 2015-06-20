#include "physics_2d_server_wrap_mt.h"

#include "os/os.h"

void Physics2DServerWrapMT::thread_exit() {

	exit=true;
}

void Physics2DServerWrapMT::thread_step(float p_delta) {

	physics_2d_server->step(p_delta);
	step_sem->post();

}

void Physics2DServerWrapMT::_thread_callback(void *_instance) {

	Physics2DServerWrapMT *vsmt = reinterpret_cast<Physics2DServerWrapMT*>(_instance);


	vsmt->thread_loop();
}

void Physics2DServerWrapMT::thread_loop() {

	server_thread=Thread::get_caller_ID();

	OS::get_singleton()->make_rendering_thread();

	physics_2d_server->init();

	exit=false;
	step_thread_up=true;
	while(!exit) {
		// flush commands one by one, until exit is requested
		command_queue.wait_and_flush_one();
	}

	command_queue.flush_all(); // flush all

	physics_2d_server->finish();

}


/* EVENT QUEUING */


void Physics2DServerWrapMT::step(float p_step) {

	if (create_thread) {

		command_queue.push( this, &Physics2DServerWrapMT::thread_step,p_step);
	} else {

		command_queue.flush_all(); //flush all pending from other threads
		physics_2d_server->step(p_step);
	}
}

void Physics2DServerWrapMT::sync() {

	if (step_sem) {
		if (first_frame)
			first_frame=false;
		else
			step_sem->wait(); //must not wait if a step was not issued
	}
	physics_2d_server->sync();;
}

void Physics2DServerWrapMT::flush_queries(){

	physics_2d_server->flush_queries();
}

void Physics2DServerWrapMT::end_sync() {

	physics_2d_server->end_sync();;
}

void Physics2DServerWrapMT::init() {

	if (create_thread) {

		step_sem = Semaphore::create();
		print_line("CREATING PHYSICS 2D THREAD");
		//OS::get_singleton()->release_rendering_thread();
		if (create_thread) {
			thread = Thread::create( _thread_callback, this );
			print_line("STARTING PHYISICS 2D THREAD");
		}
		while(!step_thread_up) {
			OS::get_singleton()->delay_usec(1000);
		}
		print_line("DONE PHYSICS 2D THREAD");
	} else {

		physics_2d_server->init();
	}

}

void Physics2DServerWrapMT::finish() {


	if (thread) {

		command_queue.push( this, &Physics2DServerWrapMT::thread_exit);
		Thread::wait_to_finish( thread );
		memdelete(thread);

/*
		shape_free_cached_ids();
		area_free_cached_ids();
		body_free_cached_ids();
		pin_joint_free_cached_ids();
		groove_joint_free_cached_ids();
		damped_string_free_cached_ids();
*/
		thread=NULL;
	} else {
		physics_2d_server->finish();
	}

	if (step_sem)
		memdelete(step_sem);

}


Physics2DServerWrapMT::Physics2DServerWrapMT(Physics2DServer* p_contained,bool p_create_thread) : command_queue(p_create_thread) {

	physics_2d_server=p_contained;
	create_thread=p_create_thread;
	thread=NULL;
	step_sem=NULL;
	step_pending=0;
	step_thread_up=false;
	alloc_mutex=Mutex::create();

	shape_pool_max_size=GLOBAL_DEF("core/thread_rid_pool_prealloc",20);
	area_pool_max_size=GLOBAL_DEF("core/thread_rid_pool_prealloc",20);
	body_pool_max_size=GLOBAL_DEF("core/thread_rid_pool_prealloc",20);
	pin_joint_pool_max_size=GLOBAL_DEF("core/thread_rid_pool_prealloc",20);
	groove_joint_pool_max_size=GLOBAL_DEF("core/thread_rid_pool_prealloc",20);
	damped_spring_joint_pool_max_size=GLOBAL_DEF("core/thread_rid_pool_prealloc",20);

	if (!p_create_thread) {
		server_thread=Thread::get_caller_ID();
	} else {
		server_thread=0;
	}

	main_thread = Thread::get_caller_ID();
	first_frame=true;
}


Physics2DServerWrapMT::~Physics2DServerWrapMT() {

	memdelete(physics_2d_server);
	memdelete(alloc_mutex);
	//finish();

}


