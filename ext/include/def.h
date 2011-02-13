// for defining ruby classes and methods convenience

#define DECL0(f) \
	static VALUE f(VALUE self)

#define DECL1(f, x) \
	static VALUE f(VALUE self, VALUE x)

#define DECL2(f, x, y) \
	static VALUE f(VALUE self, VALUE x, VALUE y)

#define DECL3(f, x, y, z) \
	static VALUE f(VALUE self, VALUE x, VALUE y, VALUE z)

#define DECL4(f, x, y, z, a) \
	static VALUE f(VALUE self, VALUE x, VALUE y, VALUE z, VALUE a)

#define DECL5(f, x, y, z, a, b) \
	static VALUE f(VALUE self, VALUE x, VALUE y, VALUE z, VALUE a, VALUE b)

#define DECL6(f, x, y, z, a, b, c) \
	static VALUE f(VALUE self, VALUE x, VALUE y, VALUE z, VALUE a, VALUE b, VALUE c)

#define DECL7(f, x, y, z, a, b, c, d) \
	static VALUE f(VALUE self, VALUE x, VALUE y, VALUE z, VALUE a, VALUE b, VALUE c, VALUE d)

#define DECL8(f, x, y, z, a, b, c, d, e) \
	static VALUE f(VALUE self, VALUE x, VALUE y, VALUE z, VALUE a, VALUE b, VALUE c, VALUE d, VALUE e)

#define DEF(klass, method, func, num) \
	rb_define_method(klass, method, RUBY_METHOD_FUNC(func), num);

#define MODULE_DEF(module, method, func, num) \
	rb_define_module_function(module, method, RUBY_METHOD_FUNC(func), num);

#define ATTR_READER(klass, attr) \
	rb_define_method(klass, #attr, RUBY_METHOD_FUNC(attr), 0)

#define ATTR_WRITER(klass, attr) \
	rb_define_method(klass, (#attr"="), RUBY_METHOD_FUNC(attr##_eq), 1)

#define ATTR_ACCESSOR(klass, attr) \
	attr_reader(klass, attr);\
	attr_writer(klass, attr)

