#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <portable-file-dialogs.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ShaderProgram.h>
#include <Model.h>
#include <stb_image.h>


// Application constants
//	Screen constants, adjust it to your native resolution.
//	TODO find a way to get this automatically, I have been unable to so far.
unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;
float ASP_RATIO = 1.777f;
//	Model matrix globally defined as it is directly controlled by the user
glm::mat4 modelMatrix = glm::mat4(1.0f);
//	Control sensitivities
//	TODO set these from the GUI
float rotatingAngle = 5.0f; // degrees, set to preference
float translationalOffset = 0.25f; // unitless, set to preference
float scaleFactor = 0.05f;
//	Window constants
bool PBRWindowOpen = false;


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

	// Handle zooming in and out
	if (key == GLFW_KEY_Z && action == GLFW_REPEAT) {
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f + scaleFactor));
	}
	if (key == GLFW_KEY_X && action == GLFW_REPEAT) {
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f - scaleFactor));
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


	// View and projection matrices
	glm::mat4 viewMatrix = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 10.0f),
		glm::vec3(0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	glm::mat4 projectionMatrix = glm::perspective(
		glm::radians(45.0f),
		ASP_RATIO,
		0.1f,
		100.0f
	);
	// Lighting
	glm::vec3 lightPositions[] = {
		glm::vec3(-10.0f, 10.0f, 0.0f),
		glm::vec3(10.0f, 10.0f, 0.0f),
		glm::vec3(-10.0f, -10.0f, 0.0f),
		glm::vec3(-10.0f, -10.0f, 0.0f)
	};
	glm::vec3 lightColor = glm::vec3(1.0f);
	glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 10.0f);
	// Albedo map handle
	unsigned int albedoMap;

	// Create albedo map
	int width, height, nrChannels;
	glGenTextures(1, &albedoMap);
	glBindTexture(GL_TEXTURE_2D, albedoMap); 
	unsigned char* data = stbi_load("defaultModel/layered-rock1-albedo.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format;
		if (nrChannels == 3) format = GL_RGB;
		else if (nrChannels == 4) format = GL_RGBA;
		else if (nrChannels == 1) format = GL_RED;
		else {
			std::cout << "# channels: " << nrChannels << std::endl;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);	


	// Load model and shaders
	glEnable(GL_DEPTH_TEST);
	ShaderProgram mainSP = ShaderProgram("shaders/main.vs", "shaders/main.fs");
	Model model = Model("defaultModel/crate.3ds");


	// Main render loop
	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::Button("Open Model")) {
				pfd::open_file f = pfd::open_file("Select your file: ");
				model = Model(f.result()[0].c_str());
			}
			if (ImGui::Button("Choose Albedo Map")) {
				pfd::open_file f = pfd::open_file("Select albedo map");
				glDeleteTextures(1, &albedoMap);
				int width, height, nrChannels;
				glGenTextures(1, &albedoMap);
				glBindTexture(GL_TEXTURE_2D, albedoMap);
				unsigned char* data = stbi_load(f.result()[0].c_str(), &width, &height, &nrChannels, 0);
				if (data)
				{
					GLenum format;
					if (nrChannels == 3) format = GL_RGB;
					else if (nrChannels == 4) format = GL_RGBA;
					else if (nrChannels == 1) format = GL_RED;
					else {
						std::cout << "# channels: " << nrChannels << std::endl;
					}

					glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
					glGenerateMipmap(GL_TEXTURE_2D);
				}
				else
				{
					std::cout << "Failed to load texture" << std::endl;
				}
				stbi_image_free(data);
			}
			ImGui::EndMainMenuBar();
		} 


		// Set vertex shader uniforms
		mainSP.use();
		mainSP.setUniformMat4("model", modelMatrix);
		mainSP.setUniformMat3("inverseModel", glm::mat3(glm::transpose(glm::inverse(modelMatrix))));
		mainSP.setUniformMat4("view", viewMatrix);
		mainSP.setUniformMat4("projection", projectionMatrix);

		// Set fragment shader uniforms
		//	Set albedo map
		mainSP.setUniformInt("albedoMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, albedoMap);
		//	Set light positions
		for (int i = 0; i < 4; i++) {
			std::string temp = "lightPositions[";
			temp += std::to_string(i) + "]";
			mainSP.setUniformVec3(temp.c_str(), lightPositions[i]);
		}
		mainSP.setUniformVec3("lightColor", lightColor);
		mainSP.setUniformVec3("cameraPosition", cameraPosition);
		// Draw model
		model.draw();


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