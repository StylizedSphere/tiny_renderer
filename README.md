Project progress:
+ Day 1: Bresenham's line algorithm
+ Day 2: Coloring model + Add lighting vector
In each triangle, using bary centric to check whether a pixel is out of bound. If not, color it. 
Create one vector for light_direction, the intensity is the dot product of normal_vector and light_direction for each triangle.
Output to tga image.
+ Day 3: Z buffer
+ Day 4-6: ModelMatrix, ProjectionMatrix, ViewPort + Texture, Shader.

![image](https://user-images.githubusercontent.com/30485720/122373057-63ff1f80-cf8b-11eb-8795-327080096e28.png) <br> <br>

However, this diablo model is prebuilt, which contains all the vertices and texture uv for mapping.
How does texture mapping actually works?
<br>
Texture parameterization with Barycentric Mappings:
Vertice x, uv coordinates calculated by this function:
![image](https://user-images.githubusercontent.com/30485720/122361348-5e044100-cf81-11eb-9ea0-441313b89329.png)
<br>
lambda calculated by this function: <br>
![image](https://user-images.githubusercontent.com/30485720/122361402-678da900-cf81-11eb-9bc9-245f4e01bfae.png)
<br>
![image](https://user-images.githubusercontent.com/30485720/122361420-69f00300-cf81-11eb-9163-dbb56b6ca176.png)
![image](https://user-images.githubusercontent.com/30485720/122364509-05827300-cf84-11eb-8544-b4df43fb8a95.png)
N means neighbor
<br>
Example: <br>
![image](https://user-images.githubusercontent.com/30485720/122372635-079c0000-cf8b-11eb-93e8-aa47dfa97802.png) <br>
![image](https://user-images.githubusercontent.com/30485720/122372612-023eb580-cf8b-11eb-945c-11373cd9702e.png) <br>
<br>


Reference:
https://www.inf.usi.ch/hormann/papers/Hormann.2008.MPT.pdf
