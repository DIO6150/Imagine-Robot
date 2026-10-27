
#include <Robot/Simulation/Simulation.hpp>

inline void initOpenGLContext(GLFWwindow *& window, std::string title, uint32_t width, uint32_t height, bool fullscreen=false)
{
	if (!glfwInit()) exit (-1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	if (fullscreen)
	{
		window = glfwCreateWindow(width, height, title.c_str(), glfwGetPrimaryMonitor(), NULL);
	}
	else
	{
		window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	}

	if (!window)
	{
		glfwTerminate ();
		exit (-1);
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Couldn't initialize GLAD.\n";
		exit(-1);
	}

	glfwSwapInterval(0);

	glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

JSONParserStatus Simulation::executeDirective(Directive instructions, DirectiveTrace & trace)
{
	/* OPENGL INITIALISATION */
	initOpenGLContext(window_, "Imagine Robot", 900, 900);

	//std::cout << "Script, environnement and agent ready." << std::endl;
	JSONParserStatus statusStart = playground_.start(window_, instructions);
	if (statusStart != JSONParserStatus::Ok)
		return statusStart;

	trace.carte = playground_.getMapName();
	trace.scenario = playground_.getScriptName();

	while(!playground_.agentIsOver())
	{
		playground_.tick();

		glfwPollEvents();
		
		if (glfwWindowShouldClose(window_)) break;
		
		glClear(GL_COLOR_BUFFER_BIT);
		playground_.draw();
		glfwSwapBuffers(window_);
	}

	trace.pas = playground_.steps();
	trace.livraisons = playground_.deliveries();
	
	return JSONParserStatus::Ok;
}
