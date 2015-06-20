/*************************************************************************/
/*  test_math.cpp                                                        */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2015 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#include "test_math.h"
#include "ustring.h"
#include "print_string.h"
#include "transform.h"
#include "matrix3.h"
#include "math_funcs.h"
#include "camera_matrix.h"
#include "scene/main/node.h"
#include "variant.h"
#include "servers/visual/shader_language.h"
#include "os/keyboard.h"
#include "scene/resources/texture.h"
#include "vmap.h"
#include "os/os.h"
namespace TestMath {


void test_vec(Plane p_vec) {


	CameraMatrix cm;
	cm.set_perspective(45,1,0,100);
	Plane v0=cm.xform4(p_vec);

	print_line("out: "+v0);
	v0.normal.z = (v0.d/100.0 *2.0-1.0) * v0.d;
	print_line("out_F: "+v0);


/*v0: 0, 0, -0.1, 0.1
v1: 0, 0, 0, 0.1
fix: 0, 0, 0, 0.1
v0: 0, 0, 1.302803, 1.5
v1: 0, 0, 1.401401, 1.5
fix: 0, 0, 1.401401, 1.5
v0: 0, 0, 25.851850, 26
v1: 0, 0, 25.925926, 26
fix: 0, 0, 25.925924, 26
v0: 0, 0, 49.899902, 50
v1: 0, 0, 49.949947, 50
fix: 0, 0, 49.949951, 50
v0: 0, 0, 100, 100
v1: 0, 0, 100, 100
fix: 0, 0, 100, 100
*/     


}


MainLoop* test() {

	{


	//	print_line("NUM: "+itos(237641278346127));
		print_line("NUM: "+itos(-128));
		return NULL;

	}


	{
		Vector3 v(1,2,3);
		v.normalize();
		float a=0.3;

		//Quat q(v,a);
		Matrix3 m(v,a);

		Vector3 v2(7,3,1);
		v2.normalize();
		float a2=0.8;

		//Quat q(v,a);
		Matrix3 m2(v2,a2);

		Quat q=m;
		Quat q2=m2;

		Matrix3 m3 = m.inverse() * m2;
		Quat q3 = (q.inverse() * q2);//.normalized();

		print_line(Quat(m3));
		print_line(q3);

		print_line("before v: "+v+" a: "+rtos(a));
		q.get_axis_and_angle(v,a);
		print_line("after v: "+v+" a: "+rtos(a));
	}

	return NULL;
	String ret;

	List<String> args;
	args.push_back("-l");
	Error err = OS::get_singleton()->execute("/bin/ls",args,true,NULL,&ret);
	print_line("error: "+itos(err));
	print_line(ret);

	return NULL;
	Matrix3 m3;
	m3.rotate(Vector3(1,0,0),0.2);
	m3.rotate(Vector3(0,1,0),1.77);
	m3.rotate(Vector3(0,0,1),212);
	Matrix3 m32;
	m32.set_euler(m3.get_euler());
	print_line("ELEULEEEEEEEEEEEEEEEEEER: "+m3.get_euler()+" vs "+m32.get_euler());


	return NULL;

	{

		Dictionary d;
		d["momo"]=1;
		Dictionary b=d;
		b["44"]=4;
	}



	return NULL;
	print_line("inters: "+rtos(Geometry::segment_intersects_circle(Vector2(-5,0),Vector2(-2,0),Vector2(),1.0)));



	print_line("cross: "+Vector3(1,2,3).cross(Vector3(4,5,7)));
	print_line("dot: "+rtos(Vector3(1,2,3).dot(Vector3(4,5,7))));
	print_line("abs: "+Vector3(-1,2,-3).abs());
	print_line("distance_to: "+rtos(Vector3(1,2,3).distance_to(Vector3(4,5,7))));
	print_line("distance_squared_to: "+rtos(Vector3(1,2,3).distance_squared_to(Vector3(4,5,7))));
	print_line("plus: "+(Vector3(1,2,3)+Vector3(Vector3(4,5,7))));
	print_line("minus: "+(Vector3(1,2,3)-Vector3(Vector3(4,5,7))));
	print_line("mul: "+(Vector3(1,2,3)*Vector3(Vector3(4,5,7))));
	print_line("div: "+(Vector3(1,2,3)/Vector3(Vector3(4,5,7))));
	print_line("mul scalar: "+(Vector3(1,2,3)*2));
	print_line("premul scalar: "+(2*Vector3(1,2,3)));
	print_line("div scalar: "+(Vector3(1,2,3)/3.0));
	print_line("length: "+rtos(Vector3(1,2,3).length()));
	print_line("length squared: "+rtos(Vector3(1,2,3).length_squared()));
	print_line("normalized: "+Vector3(1,2,3).normalized());
	print_line("inverse: "+Vector3(1,2,3).inverse());

	{
		Vector3 v(4,5,7);
		v.normalize();
		print_line("normalize: "+v);
	}

	{
		Vector3 v(4,5,7);
		v+=Vector3(1,2,3);
		print_line("+=: "+v);
	}

	{
		Vector3 v(4,5,7);
		v-=Vector3(1,2,3);
		print_line("-=: "+v);
	}

	{
		Vector3 v(4,5,7);
		v*=Vector3(1,2,3);
		print_line("*=: "+v);
	}

	{
		Vector3 v(4,5,7);
		v/=Vector3(1,2,3);
		print_line("/=: "+v);
	}

	{
		Vector3 v(4,5,7);
		v*=2.0;
		print_line("scalar *=: "+v);
	}

	{
		Vector3 v(4,5,7);
		v/=2.0;
		print_line("scalar /=: "+v);
	}



#if 0
	print_line(String("C:\\momo\\.\\popo\\..\\gongo").simplify_path());
	print_line(String("res://../popo/..//gongo").simplify_path());
	print_line(String("res://..").simplify_path());


	DVector<uint8_t> a;
	DVector<uint8_t> b;

	a.resize(20);
	b=a;
	b.resize(30);
	a=b;
#endif

#if 0
	String za = String::utf8("á");
	printf("unicode: %x\n",za[0]);
	CharString cs=za.utf8();
	for(int i=0;i<cs.size();i++) {
		uint32_t v = uint8_t(cs[i]);
		printf("%i - %x\n",i,v);
	}
	return NULL;

	print_line(String("C:\\window\\system\\momo").path_to("C:\\window\\momonga"));
	print_line(String("res://momo/sampler").path_to("res://pindonga"));
	print_line(String("/margarito/terere").path_to("/margarito/pilates"));
	print_line(String("/algo").path_to("/algo"));
	print_line(String("c:").path_to("c:\\"));
	print_line(String("/").path_to("/"));


	print_line(itos(sizeof(Variant)));
	return NULL;

	Vector<StringName> path;
	path.push_back("three");
	path.push_back("two");
	path.push_back("one");
	path.push_back("comeon");
	path.revert();

	NodePath np(path,true);

	print_line(np);


	return NULL;

	bool a=2;

	print_line(Variant(a));


	Matrix32 mat2_1;
	mat2_1.rotate(0.5);
	Matrix32 mat2_2;
	mat2_2.translate(Vector2(1,2));
	Matrix32 mat2_3 = mat2_1 * mat2_2;
	mat2_3.affine_invert();

	print_line(mat2_3.elements[0]);
	print_line(mat2_3.elements[1]);
	print_line(mat2_3.elements[2]);



	Transform mat3_1;
	mat3_1.basis.rotate(Vector3(0,0,1),0.5);
	Transform mat3_2;
	mat3_2.translate(Vector3(1,2,0));
	Transform mat3_3 = mat3_1 * mat3_2;
	mat3_3.affine_invert();

	print_line(mat3_3.basis.get_axis(0));
	print_line(mat3_3.basis.get_axis(1));
	print_line(mat3_3.origin);

#endif
	return NULL;

}

}
