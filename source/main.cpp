#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <portable-file-dialogs.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ShaderProgram.h>
#include <Camera.h>
#include <Model.h>
#include <Texture2D.h>


// Screen constants, adjust it to your native resolution.
// TODO find a way to get this automatically, I have been unable to so far.
unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;
float ASP_RATIO = 1.777f;
// Model matrix globally defined as it is directly controlled by the user
glm::mat4 modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
// Control sensitivities
// TODO set these from the GUI
float rotatingAngle = 5.0f; // degrees, set to preference
float translationalOffset = 0.25f; // unitless, set to preference


// Callback function that controls movement of the model
// TODO Maybe find a way to do it without glm::inverse()
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// Handle roll around x-axis
	if (key == GLFW_KEY_W && action == GLFW_REPEAT) {
		modelMatrix = glm::rotate(
			modelMatrix,
			glm::radians(rotatingAngle),
			glm::mat3(glm::inverse(modelMatrix)) * glm::vec3(1.0f, 0.0f, 0.0f)
		);
	}
	if (key == GLFW_KEY_S && action == GLFW_REPEAT) {
		modelMatrix = glm::rotate(
			modelMatrix, 
			glm::radians(-rotatingAngle),
			glm::mat3(glm::inverse(modelMatrix)) * glm::vec3(1.0f, 0.0f, 0.0f)
		);
	}
	// Handle roll around y-axis
	if (key == GLFW_KEY_A && action == GLFW_REPEAT) {
		modelMatrix = glm::rotate(
			modelMatrix,
			glm::radians(rotatingAngle),
			glm::mat3(glm::inverse(modelMatrix)) * glm::vec3(0.0f, 1.0f, 0.0f)
		);
	}
	if (key == GLFW_KEY_D && action == GLFW_REPEAT) {
		modelMatrix = glm::rotate(
			modelMatrix,
			glm::radians(-rotatingAngle),
			glm::mat3(glm::inverse(modelMatrix)) * glm::vec3(0.0f, 1.0f, 0.0f)
		);
	}
	// Handle roll around z-axis
	if (key == GLFW_KEY_Q && action == GLFW_REPEAT) {
		modelMatrix = glm::rotate(
			modelMatrix,
			glm::radians(rotatingAngle),
			glm::mat3(glm::inverse(modelMatrix)) * glm::vec3(0.0f, 0.0f, 1.0f)
		);
	}
	if (key == GLFW_KEY_E && action == GLFW_REPEAT) {
		modelMatrix = glm::rotate(
			modelMatrix,
			glm::radians(-rotatingAngle),
			glm::mat3(glm::inverse(modelMatrix)) * glm::vec3(0.0f, 0.0f, 1.0f)
		);
	}

	// Handle translational movement about x-axis
	if (key == GLFW_KEY_RIGHT && action == GLFW_REPEAT) {
		modelMatrix = glm::translate(
			modelMatrix,
			translationalOffset * (glm::mat3(glm::inverse(modelMatrix)) * glm::vec3(1.0f, 0.0f, 0.0f))
		);
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_REPEAT) {
		modelMatrix = glm::translate(
			modelMatrix,
			translationalOffset * (glm::mat3(glm::inverse(modelMatrix)) * glm::vec3(-1.0f, 0.0f, 0.0f))
		);
	}
	// Handle translational movement about y-axis
	if (key == GLFW_KEY_UP && action == GLFW_REPEAT) {
		modelMatrix = glm::translate(
			modelMatrix,
			translationalOffset * (glm::mat3(glm::inverse(modelMatrix)) * glm::vec3(0.0f, 1.0f, 0.0f))
		);
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_REPEAT) {
		modelMatrix = glm::translate(
			modelMatrix,
			translationalOffset * (glm::mat3(glm::inverse(modelMatrix)) * glm::vec3(0.0f, -1.0f, 0.0f))
		);
	}
}


int main() {
	// Initialize GLFW and GLAD
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Loading Model...", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwMaximizeWindow(window);
	glfwSetKeyCallback(window, keyCallback);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	// Initialize Dear ImGUI with the provided OpenGL bindings.
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale *= 1.8f;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();


	// Load main shader program
	glEnable(GL_DEPTH_TEST);
	ShaderProgram mainSP = ShaderProgram("shaders/main.vs", "shaders/main.fs");
	Model model = Model("defaultModel/dragon.dae");


	// Main render loop
	while (!glfwWindowShouldClose(window)) {
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();		
		ImGui::NewFrame();


		mainSP.use();
		mainSP.setUniformMat4("model", modelMatrix);
		mainSP.setUniformMat4("view", glm::lookAt(
			glm::vec3(0.0f, 0.0f, 10.0f),
			glm::vec3(0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		));
		mainSP.setUniformMat4("projection", glm::perspective(
			glm::radians(45.0f),
			ASP_RATIO,
			0.1f,
			100.0f
		));
		model.draw();


		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::Button("Open")) {
				auto f = pfd::open_file("Select your file: ");
				model = Model(f.result()[0].c_str());
			}
			ImGui::EndMainMenuBar();
		}	

		
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// Terminate Dear ImGUI and GLFW
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}