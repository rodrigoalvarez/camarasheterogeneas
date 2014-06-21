// PointCloudViewer Unity3D - http://unitycoder.com/blog/

using UnityEngine;
using System.Collections.Generic;
using System.IO;
//using System.Collections;
//using System;
//using System.IO;

public class mPointCloudViewer1 : MonoBehaviour {
	
	public Material material;
	public Material material2;
	private int vertexCount; // this should match the amount of points from file
	private int vertexCount2; // this should match the amount of points from file
	private int instanceCount = 1; // no need to adjust (otherwise you have instanceCount * vertexCount amount of objects..
	private int instanceCount2 = 1; // no need to adjust (otherwise you have instanceCount * vertexCount amount of objects..
	
	private ComputeBuffer bufferPoints;
	private ComputeBuffer bufferPos;
	
	
	private ComputeBuffer bufferPoints2;
	private ComputeBuffer bufferPos2;
	
	private Vector3[] pos;
	private Vector3[] verts;
	private Vector3[] pos2;
	private Vector3[] verts2;
	private Vector3[] maxPoint;
	private Vector3[] minPoint;
	
	
	private GameObject[] spheres;
	public Transform target2;
	bool select = false;
	Vector3 posStart = Vector3.zero;
	Vector3 posStop  = Vector3.zero;
	public TextAsset fileName;
	public TextAsset nubeA;
	public TextAsset nubeB;
	public TextAsset calibracion1;
	public TextAsset calibracion2;
	public Vector3 offset;
	float moveSpeed = 1.0f;
	//float moveSpeedMouse = 2.0f;
	Vector3 puntoReferencia = Vector3.zero;
	Vector3 seleccionUno = Vector3.zero;
	Vector3 seleccionDos = Vector3.zero;
	Vector3 seleccionTres = Vector3.zero;
	int numeroSeleccion = 1;
	public GUITexture textura;
	int tamano =0;
	
	
	double[,] matrizV = new double[3,3];
	double[,] matrizT = new double[3,3];
	
	StreamWriter sw;
	
	void Start () 
	{
		bool transforma = false;
		sw  = new StreamWriter("puntosTransformadosUnity.txt");
		string[] puntos1 = calibracion1.text.Split('\n');
		string[] puntos2 = calibracion2.text.Split('\n');
		for (int i = 0; i < 3; ++i)
		{
			string[] xyz = puntos1[i].Split(' ');
			matrizV[i,0] = double.Parse(xyz[0]);
			matrizV[i,1] = double.Parse(xyz[1]);
			matrizV[i,2] = double.Parse(xyz[2]);
			
			xyz = puntos2[i].Split(' ');
			matrizT[i,0] = double.Parse(xyz[0]);
			matrizT[i,1] = double.Parse(xyz[1]);
			matrizT[i,2] = double.Parse(xyz[2]);
			
		}
		textura.enabled = false;
		// load from file
		string[] xyzline = nubeA.text.Split('\n');
		Debug.Log("Points in file:" + xyzline.Length);

		// init arrays, using value from the file
		vertexCount = xyzline.Length;
		//vertexCount = 500;		
		
		verts = new Vector3[vertexCount];
		//spheres = new GameObject[vertexCount];
		// get xyz values
		bool paso = false;
		if (transforma){
			Debug.Log(" Nube transformada ");
		}
		else{
			Debug.Log(" Nube sin transformar ");
		}
		for (var i = 0; i < vertexCount; ++i)
		{
			//Debug.Log(xyzline[i]);
			string[] xyz = xyzline[i].Split(' ');
			if (xyz[0] != "nan"){
				
				float x = float.Parse(xyz[0]);
				float y = float.Parse(xyz[1]);
				float z = float.Parse(xyz[2]);
				//verts[i] = new Vector3(x,y,z);
				double[] p = {x,y,z};
				double[] pt = new double[3];
				if (transforma){
					Solve(matrizV,matrizT,p,ref pt);
					verts[i] = new Vector3((float)pt[0],(float)pt[1],(float)pt[2]);
				}
				else{
					verts[i] = new Vector3(x,y,z);
				}
				if (!paso){
					puntoReferencia = new Vector3(	verts[i].x ,
													verts[i].y ,
													verts[i].z );
					Debug.Log(puntoReferencia.x + " - " + puntoReferencia.y + " - " + puntoReferencia.z);
					paso = true;
				}
			}
			else{
				float x = 0;
				float y = 0;
				float z = 0;
				verts[i] = new Vector3(x,y,z);
			}
		}
		sw.Close();
		ReleaseBuffers ();
		
		bufferPoints = new ComputeBuffer (vertexCount, 12);
		bufferPoints.SetData (verts);
		//bufferPoints.SetData (spheres);
		material.SetBuffer ("buf_Points", bufferPoints);

		pos = new Vector3[instanceCount];

		pos[0] = new Vector3(0.0f,0,0);
		
		bufferPos = new ComputeBuffer (instanceCount, 12);
		material.SetBuffer ("buf_Positions", bufferPos);

		bufferPos.SetData (pos);
		
		
		/******************************************************/
		/*****************************************************/
		string[] xyzline2 = nubeB.text.Split('\n');

		// init arrays, using value from the file
		vertexCount2 = xyzline2.Length;
		//vertexCount = 500;		
		
		verts2 = new Vector3[vertexCount2];
		//spheres = new GameObject[vertexCount];
		// get xyz values
		for (var i = 0; i < vertexCount2; ++i)
		{
			//Debug.Log(xyzline[i]);
			string[] xyz = xyzline2[i].Split(' ');
			if (xyz[0] != "nan"){
				
				float x = float.Parse(xyz[0]);
				float y = float.Parse(xyz[1]);
				float z = float.Parse(xyz[2]);
				//verts[i] = new Vector3(x,y,z);
				double[] p = {x,y,z};
				double[] pt = new double[3];
				verts2[i] = new Vector3(x,y,z);
			}
			else{
				float x = 0;
				float y = 0;
				float z = 0;
				verts2[i] = new Vector3(x,y,z);
			}
		}
		ReleaseBuffers2 ();
		
		bufferPoints2 = new ComputeBuffer (vertexCount2, 12);
		bufferPoints2.SetData (verts2);
		//bufferPoints.SetData (spheres);
		material2.SetBuffer ("buf_Points", bufferPoints2);

		pos2 = new Vector3[instanceCount];

		pos2[0] = new Vector3(0.0f,0,0);
		
		bufferPos2 = new ComputeBuffer (instanceCount2, 12);
		material2.SetBuffer ("buf_Positions", bufferPos2);

		bufferPos2.SetData (pos2);
		
		
		
	}
	
	private void ReleaseBuffers() 
	{
		if (bufferPoints != null) bufferPoints.Release();
		bufferPoints = null;
		if (bufferPos != null) bufferPos.Release();
		bufferPos = null;
	}
	
	private void ReleaseBuffers2() 
	{
		if (bufferPoints2 != null) bufferPoints2.Release();
		bufferPoints2 = null;
		if (bufferPos2 != null) bufferPos2.Release();
		bufferPos2 = null;
	}
	
	void OnDisable() 
	{
		ReleaseBuffers ();
	}


	void OnRenderObject () 
	{
		material.SetPass (0);
		Graphics.DrawProcedural (MeshTopology.Points , vertexCount, instanceCount);
		material2.SetPass (0);
		Graphics.DrawProcedural (MeshTopology.Points , vertexCount2, instanceCount2);
	}
	

	
	public void Solve(double[,] V, double[,] T, double[] R, ref double[] S) {
        double alfa = -((-V[1,2] * V[2,1] * R[0] + V[1,1] * V[2,2] * R[0] + V[1,2] * V[2,0] * R[1] - V[1,0] * V[2,2] * R[1] - V[1,1] * V[2,0] * R[2] + V[1,0] * V[2,1] * R[2]) /
            (V[0,2] * V[1,1] * V[2,0] - V[0,1] * V[1,2] * V[2,0] - V[0,2] * V[1,0] * V[2,1] + V[0,0] * V[1,2] * V[2,1] + V[0,1] * V[1,0] * V[2,2] - V[0,0] * V[1,1] * V[2,2]));

        double beta = -((V[0, 2] * V[2, 1] * R[0] - V[0, 1] * V[2, 2] * R[0] - V[0, 2] * V[2, 0] * R[1] + V[0, 0] * V[2, 2] * R[1] + V[0, 1] * V[2, 0] * R[2] - V[0, 0] * V[2, 1] * R[2]) /
            (V[0, 2] * V[1, 1] * V[2, 0] - V[0, 1] * V[1, 2] * V[2, 0] - V[0, 2] * V[1, 0] * V[2, 1] + V[0, 0] * V[1, 2] * V[2, 1] + V[0, 1] * V[1, 0] * V[2, 2] - V[0, 0] * V[1, 1] * V[2, 2]));

        double gamma = -((-V[0, 2] * V[1, 1] * R[0] + V[0, 1] * V[1, 2] * R[0] + V[0, 2] * V[1, 0] * R[1] - V[0, 0] * V[1, 2] * R[1] - V[0, 1] * V[1, 0] * R[2] + V[0, 0] * V[1, 1] * R[2]) /
            (V[0, 2] * V[1, 1] * V[2, 0] - V[0, 1] * V[1, 2] * V[2, 0] - V[0, 2] * V[1, 0] * V[2, 1] + V[0, 0] * V[1, 2] * V[2, 1] + V[0, 1] * V[1, 0] * V[2, 2] - V[0, 0] * V[1, 1] * V[2, 2]));

        S[0] = alfa * T[0, 0] + beta * T[1, 0] + gamma * T[2, 0];
        S[1] = alfa * T[0, 1] + beta * T[1, 1] + gamma * T[2, 1];
        S[2] = alfa * T[0, 2] + beta * T[1, 2] + gamma * T[2, 2];
		sw.WriteLine(S[0] + " " + S[1] + " " + S[2]);
    }
	        /*public static Matrix GetRotationMatrix(this Vector3 source, Vector3 target)
        {
            float dot = Vector3.Dot(source, target);
            if (!float.IsNaN(dot))
            {
                float angle = (float)Math.Acos(dot);
                if (!float.IsNaN(angle))
                {
                    Vector3 cross = Vector3.Cross(source, target);
                    cross.Normalize();
                    Matrix rotation = Matrix.CreateFromAxisAngle(cross, angle);
                    return rotation;
                }
            }
            return Matrix.Identity;
        }*/
	
	void Update()
	{			
		//UnityEngine.Rect rectangle2 = new UnityEngine.Rect(0,-128,128,128);
		//textura.pixelInset = rectangle2;
		//Debug.Log(Camera.main.transform.rotation.ToString());
		float moveSpeed = 10.0f;
		float speedMod  = 10.0f;
		
		float smooth = 2.0f;
		float tiltAngle = 30.0f;
		
		float angleRotate = 0.5f;
	
	if (Input.GetKey(KeyCode.F) || Input.GetKey(KeyCode.G) || Input.GetKey(KeyCode.T)|| Input.GetKey(KeyCode.H))	{
		if(Input.GetKey(KeyCode.F)) {
			for(int i =0;i<vertexCount2;i++){
				verts2[i] = new Vector3(	verts2[i].x-10 , verts2[i].y ,	verts2[i].z );
			}
		}		
		if(Input.GetKey(KeyCode.H)) {
			for(int i =0;i<vertexCount2;i++){
				verts2[i] = new Vector3(	verts2[i].x+10 , verts2[i].y ,	verts2[i].z );
			}
		}
		if(Input.GetKey(KeyCode.G)) {
			for(int i =0;i<vertexCount2;i++){
				verts2[i] = new Vector3(	verts2[i].x , verts2[i].y-10 ,	verts2[i].z );
			}
		}
		if(Input.GetKey(KeyCode.T)) {
			for(int i =0;i<vertexCount2;i++){
				verts2[i] = new Vector3(	verts2[i].x , verts2[i].y+10 ,	verts2[i].z );
			}
		}
			
		ReleaseBuffers2 ();
	
		bufferPoints2 = new ComputeBuffer (vertexCount2, 12);
		bufferPoints2.SetData (verts2);
		//bufferPoints.SetData (spheres);
		material2.SetBuffer ("buf_Points", bufferPoints2);

		pos2 = new Vector3[instanceCount2];

		pos2[0] = new Vector3(0.0f,0,0);
		
		bufferPos2 = new ComputeBuffer (instanceCount2, 12);
		material2.SetBuffer ("buf_Positions", bufferPos2);

		bufferPos2.SetData (pos2);
	}
	if(Input.GetKey(KeyCode.A)) {
		Camera.main.transform.RotateAround(Vector3.zero, Vector3.up, 1);
    	Debug.Log(Mathf.Cos(Mathf.Deg2Rad * Camera.main.transform.rotation.eulerAngles.y));
	}
	if(Input.GetKey(KeyCode.D)) {
		Camera.main.transform.RotateAround(Vector3.zero, Vector3.down, 1);
    	Debug.Log(Mathf.Cos(Mathf.Deg2Rad * Camera.main.transform.rotation.eulerAngles.y));
	}
		
		if(Input.GetKey(KeyCode.W)) {
		Camera.main.transform.RotateAround(Vector3.zero, Vector3.right, 1);
	}
	if(Input.GetKey(KeyCode.S)) {
		Camera.main.transform.RotateAround(Vector3.zero, Vector3.left, 1);
	}
		
	/*if(Input.GetKey(KeyCode.W)) {
			
		Camera.main.transform.RotateAround (target2.position, Vector3.right, angleRotate);
	}
	if(Input.GetKey(KeyCode.S)) {
		Camera.main.transform.RotateAround (target2.position, Vector3.left, angleRotate);
	}
	if(Input.GetKey(KeyCode.A)) {
		Camera.main.transform.RotateAround (target2.position, Vector3.up, angleRotate);
	}
	if(Input.GetKey(KeyCode.D)) {
		Camera.main.transform.RotateAround (target2.position, Vector3.down, angleRotate);
	}*/
		
		if(Input.GetKey(KeyCode.R)) {
			numeroSeleccion = 1;
		}
		if(Input.GetKey(KeyCode.UpArrow)) {			
				Vector3 temp = new Vector3(	Camera.main.transform.position.x,
											Camera.main.transform.position.y + (moveSpeed * Mathf.Cos(Mathf.Deg2Rad * Camera.main.transform.rotation.eulerAngles.x)),
											Camera.main.transform.position.z + (moveSpeed * Mathf.Sin(Mathf.Deg2Rad * Camera.main.transform.rotation.eulerAngles.x)));
				Camera.main.transform.position = temp;
	    }
	    if(Input.GetKey(KeyCode.DownArrow)) {
			Vector3 temp = new Vector3(	Camera.main.transform.position.x,
										Camera.main.transform.position.y - (moveSpeed * Mathf.Cos(Mathf.Deg2Rad * Camera.main.transform.rotation.eulerAngles.x)),
										Camera.main.transform.position.z - (moveSpeed * Mathf.Sin(Mathf.Deg2Rad * Camera.main.transform.rotation.eulerAngles.x)));
			Camera.main.transform.position = temp;
	    } 
	    if(Input.GetKey(KeyCode.RightArrow)) {
			Vector3 temp = new Vector3(	Camera.main.transform.position.x + (moveSpeed * Mathf.Cos(Mathf.Deg2Rad * Camera.main.transform.rotation.eulerAngles.y)),
										Camera.main.transform.position.y,
										Camera.main.transform.position.z - (moveSpeed * Mathf.Sin(Mathf.Deg2Rad * Camera.main.transform.rotation.eulerAngles.y)));
			Camera.main.transform.position = temp;
		}
	    if(Input.GetKey(KeyCode.LeftArrow)) {
			Vector3 temp = new Vector3(	Camera.main.transform.position.x - (moveSpeed * Mathf.Cos(Mathf.Deg2Rad * Camera.main.transform.rotation.eulerAngles.y)),
										Camera.main.transform.position.y,
										Camera.main.transform.position.z + (moveSpeed * Mathf.Sin(Mathf.Deg2Rad * Camera.main.transform.rotation.eulerAngles.y)));
			Camera.main.transform.position = temp;
	    }
	    
		if (Input.GetAxis("Mouse ScrollWheel") > 0) {
			Vector3 temp = new Vector3(	Camera.main.transform.position.x + (moveSpeed * Mathf.Sin(Mathf.Deg2Rad * Camera.main.transform.rotation.eulerAngles.y)),
										Camera.main.transform.position.y,
										Camera.main.transform.position.z + (moveSpeed * Mathf.Cos(Mathf.Deg2Rad * Camera.main.transform.rotation.eulerAngles.y)));
			Camera.main.transform.position = temp;
		}
		if (Input.GetAxis("Mouse ScrollWheel") < 0) {
			Vector3 temp = new Vector3(	Camera.main.transform.position.x - (moveSpeed * Mathf.Sin(Mathf.Deg2Rad * Camera.main.transform.rotation.eulerAngles.y)),
										Camera.main.transform.position.y,
										Camera.main.transform.position.z - (moveSpeed * Mathf.Cos(Mathf.Deg2Rad * Camera.main.transform.rotation.eulerAngles.y)));
			Camera.main.transform.position = temp;
		}	
		if(Input.GetKey(KeyCode.LeftControl) && Input.GetMouseButton(0)) {
			if (!select) {
					textura.enabled = true;
                   select = true;
                   posStart = Input.mousePosition;
           	}
			
			Vector3 position = new Vector3(posStart.x / Screen.width,posStart.y / Screen.height,textura.transform.position.z);
           	textura.transform.position = position;
			UnityEngine.Rect rectangle = new UnityEngine.Rect(textura.pixelInset.x,-textura.pixelInset.height,Input.mousePosition.x - posStart.x, posStart.y - Input.mousePosition.y);
			textura.pixelInset = rectangle;
		}
		if(Input.GetKey(KeyCode.LeftControl) && Input.GetMouseButtonUp(0) && select) {
			select = false;
			textura.enabled = false;
            posStop = Input.mousePosition;
			//Debug.Log("Button Up");
			posStop = Input.mousePosition;			
			//Debug.Log("Start: "+posStart.x+" - "+posStart.y);
			//Debug.Log("Stop: "+posStop.x+" - "+posStop.y);
			List<Vector3> puntos = new List<Vector3>(); 
			for (int i = 0; i < vertexCount ; i++){
				Vector3 k = camera.WorldToScreenPoint (verts[i]);
				
					//Debug.Log(k.x+" "+k.y);
				if (posStart.x < posStop.x && posStart.y > posStop.y) {
					if (k.x > posStart.x && k.x < posStop.x && k.y < posStart.y && k.y > posStop.y) {
						puntos.Add(verts[i]);
						//Debug.Log(i);
					}
				}
				if (posStart.x < posStop.x && posStart.y < posStop.y) {
					if (k.x > posStart.x && k.x < posStop.x && k.y > posStart.y && k.y < posStop.y) {
						puntos.Add(verts[i]);
						//Debug.Log(i);
					}
				}
				if (posStart.x > posStop.x && posStart.y > posStop.y) {
					if (k.x < posStart.x && k.x > posStop.x && k.y < posStart.y && k.y > posStop.y) {
						puntos.Add(verts[i]);
						//Debug.Log(i);
					}
				}
				if (posStart.x > posStop.x && posStart.y < posStop.y) {
					if (k.x < posStart.x && k.x > posStop.x && k.y > posStart.y && k.y < posStop.y) {
						puntos.Add(verts[i]);
						//Debug.Log(i);
					}
				}
				
			}
				float x=0, y=0, z=0;
				foreach (Vector3 punto in puntos){
					x += punto.x/puntos.Count;
					y += punto.y/puntos.Count;
					z += punto.z/puntos.Count;
				}
				switch (numeroSeleccion){
					case 1: seleccionUno = new Vector3(x,y,z);
							Debug.Log("Punto 1 seleccionado x="+x+" y="+y+" z="+z);
							numeroSeleccion = 2;
							break;
					case 2: seleccionDos = new Vector3(x,y,z);
							Debug.Log("Punto 2 seleccionado x="+x+" y="+y+" z="+z);
							numeroSeleccion = 3;
							break;
					case 3: seleccionTres = new Vector3(x,y,z);
							Debug.Log("Punto 3 seleccionado x="+x+" y="+y+" z="+z);
							numeroSeleccion = 4;
							System.IO.File.WriteAllText("Salida.txt","{ {"+seleccionUno.x+", "+seleccionUno.y+", "+seleccionUno.z+"}, "+'\n'+
																	"{"+seleccionDos.x+", "+seleccionDos.y+", "+seleccionDos.z+"}, "+'\n'+
																	"{"+seleccionTres.x+", "+seleccionTres.y+", "+seleccionTres.z+"} }");
							break;
				}
		}
	}
}
