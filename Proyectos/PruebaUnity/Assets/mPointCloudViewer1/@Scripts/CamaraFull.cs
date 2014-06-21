using UnityEngine;
using System.IO;
using System.Collections;
using System.Collections.Generic;

public class CamaraFull : MonoBehaviour {
	
	public Material material;
	public Material material2;
	
	public TextAsset nubeA;
	public TextAsset nubeB;
	
	public GUIText message;
	
	private ComputeBuffer bufferPoints;
	private ComputeBuffer bufferPos;
	private ComputeBuffer bufferPoints2;
	private ComputeBuffer bufferPos2;
	
	private Vector3[] cloud1;
	private int nCloud1;
	private Vector3[] cloud2;
	private int nCloud2;
	private bool cloudActive = true;
	
	private int positionCloud = 0;
	
	private float moveSpeed = 10.0f;
	private float angleSpeed = 10.0f;
	private Vector3 rotateCentre;
	
	private List<Matrix4x4> listMatrix = new List<Matrix4x4>();
	
	
	private Matrix4x4 transformMatrix1 = Matrix4x4.identity;
	private Matrix4x4 transformMatrix2 = Matrix4x4.identity;
	private Matrix4x4 transformMatrixR = Matrix4x4.identity;
	private Matrix4x4 transformMatrixL = Matrix4x4.identity;
	private Matrix4x4 transformMatrixU = Matrix4x4.identity;
	private Matrix4x4 transformMatrixB = Matrix4x4.identity;
	private Matrix4x4 transformMatrixMF = Matrix4x4.identity;
	private Matrix4x4 transformMatrixMB = Matrix4x4.identity;
	private Matrix4x4 transformMatrixQ = Matrix4x4.identity;
	private Matrix4x4 transformMatrixW = Matrix4x4.identity;
	private Matrix4x4 transformMatrixE = Matrix4x4.identity;
	private Matrix4x4 transformMatrixA = Matrix4x4.identity;
	private Matrix4x4 transformMatrixS = Matrix4x4.identity;
	private Matrix4x4 transformMatrixD = Matrix4x4.identity;
	
	private Vector4 puntoUno;
	
	void Start ()
	{
		string current = System.Environment.CurrentDirectory;
		Debug.Log(current);
		message.text = "Loading cloud 1";
		nCloud1 = loadCloud (string.Format("cloud{0}.xyz", positionCloud++), ref cloud1);
		
		puntoUno = new Vector4(cloud1[0].x, cloud1[0].y, cloud1[0].z, 1);
		
		message.text = "Loading cloud 2";
		nCloud2 = loadCloud (string.Format("cloud{0}.xyz", positionCloud++), ref cloud2);
		updateCentreCloud ();
		message.text = "";
		inicializeMatrix();
		rePaint();
	}
	
	private void inicializeMatrix(){
		transformMatrixR[0,3] = moveSpeed;
		transformMatrixL[0,3] = -moveSpeed;
		transformMatrixU[1,3] = moveSpeed;
		transformMatrixB[1,3] = -moveSpeed;
		transformMatrixMF[2,3] = moveSpeed*2;
		transformMatrixMB[2,3] = -moveSpeed*2;
		
		/***************RotationS*****************/
		Matrix4x4 transformMatrixOrigen = Matrix4x4.identity;
		transformMatrixOrigen[0,3] = rotateCentre.x;
		transformMatrixOrigen[1,3] = rotateCentre.y;
		transformMatrixOrigen[2,3] = rotateCentre.z;
		
		Matrix4x4 transformMatrixOpuestoOrigen = Matrix4x4.identity;
		transformMatrixOpuestoOrigen[0,3] = -rotateCentre.x;
		transformMatrixOpuestoOrigen[1,3] = -rotateCentre.y;
		transformMatrixOpuestoOrigen[2,3] = -rotateCentre.z;
		
		//Rotation X
		Matrix4x4 transformMatrixRotacionXPlus = Matrix4x4.identity;
		transformMatrixRotacionXPlus[1,1] = Mathf.Cos(angleSpeed * Mathf.Deg2Rad);
		transformMatrixRotacionXPlus[1,2] = -Mathf.Sin(angleSpeed * Mathf.Deg2Rad);
		transformMatrixRotacionXPlus[2,1] = Mathf.Sin(angleSpeed * Mathf.Deg2Rad);
		transformMatrixRotacionXPlus[2,2] = Mathf.Cos(angleSpeed * Mathf.Deg2Rad);
		
		Matrix4x4 transformMatrixRotacionXMinium = Matrix4x4.identity;
		transformMatrixRotacionXMinium[1,1] = Mathf.Cos(-angleSpeed * Mathf.Deg2Rad);
		transformMatrixRotacionXMinium[1,2] = -Mathf.Sin(-angleSpeed * Mathf.Deg2Rad);
		transformMatrixRotacionXMinium[2,1] = Mathf.Sin(-angleSpeed * Mathf.Deg2Rad);
		transformMatrixRotacionXMinium[2,2] = Mathf.Cos(-angleSpeed * Mathf.Deg2Rad);
		
		transformMatrixW = transformMatrixOrigen * transformMatrixRotacionXPlus * transformMatrixOpuestoOrigen;
		transformMatrixS = transformMatrixOrigen * transformMatrixRotacionXMinium * transformMatrixOpuestoOrigen;
		
		//Rotation Y
		Matrix4x4 transformMatrixRotacionYPlus = Matrix4x4.identity;
		transformMatrixRotacionYPlus[0,0] = Mathf.Cos(angleSpeed * Mathf.Deg2Rad);
		transformMatrixRotacionYPlus[0,2] = Mathf.Sin(angleSpeed * Mathf.Deg2Rad);
		transformMatrixRotacionYPlus[2,0] = -Mathf.Sin(angleSpeed * Mathf.Deg2Rad);
		transformMatrixRotacionYPlus[2,2] = Mathf.Cos(angleSpeed * Mathf.Deg2Rad);
		
		Matrix4x4 transformMatrixRotacionYMinium = Matrix4x4.identity;
		transformMatrixRotacionYMinium[0,0] = Mathf.Cos(-angleSpeed * Mathf.Deg2Rad);
		transformMatrixRotacionYMinium[0,2] = Mathf.Sin(-angleSpeed * Mathf.Deg2Rad);
		transformMatrixRotacionYMinium[2,0] = -Mathf.Sin(-angleSpeed * Mathf.Deg2Rad);
		transformMatrixRotacionYMinium[2,2] = Mathf.Cos(-angleSpeed * Mathf.Deg2Rad);
		
		transformMatrixD = transformMatrixOrigen * transformMatrixRotacionYPlus * transformMatrixOpuestoOrigen;
		transformMatrixA = transformMatrixOrigen * transformMatrixRotacionYMinium * transformMatrixOpuestoOrigen;
		
		//Rotation Z
		Matrix4x4 transformMatrixRotacionZPlus = Matrix4x4.identity;
		transformMatrixRotacionZPlus[0,0] = Mathf.Cos(angleSpeed * Mathf.Deg2Rad);
		transformMatrixRotacionZPlus[0,1] = Mathf.Sin(angleSpeed * Mathf.Deg2Rad);
		transformMatrixRotacionZPlus[1,0] = -Mathf.Sin(angleSpeed * Mathf.Deg2Rad);
		transformMatrixRotacionZPlus[1,1] = Mathf.Cos(angleSpeed * Mathf.Deg2Rad);
		
		Matrix4x4 transformMatrixRotacionZMinium = Matrix4x4.identity;
		transformMatrixRotacionZMinium[0,0] = Mathf.Cos(-angleSpeed * Mathf.Deg2Rad);
		transformMatrixRotacionZMinium[0,1] = Mathf.Sin(-angleSpeed * Mathf.Deg2Rad);
		transformMatrixRotacionZMinium[1,0] = -Mathf.Sin(-angleSpeed * Mathf.Deg2Rad);
		transformMatrixRotacionZMinium[1,1] = Mathf.Cos(-angleSpeed * Mathf.Deg2Rad);
		
		transformMatrixE = transformMatrixOrigen * transformMatrixRotacionZPlus * transformMatrixOpuestoOrigen;
		transformMatrixQ = transformMatrixOrigen * transformMatrixRotacionZMinium * transformMatrixOpuestoOrigen;
	}
	
	private void rePaint() {
		//Vector3[] cloud = cloudActive ? cloud1 : cloud2;
		//int nCloud = cloudActive ? nCloud1 : nCloud2;
		if (cloud1 != null){
			ReleaseBuffers ();
			bufferPoints = new ComputeBuffer (nCloud1, 12);
			bufferPoints.SetData (cloud1);
			material.SetBuffer ("buf_Points", bufferPoints);
			Vector3[] pos = new Vector3[1];
			pos[0] = new Vector3(0.0f, 0, 0);
			bufferPos = new ComputeBuffer (1, 12);
			material.SetBuffer ("buf_Positions", bufferPos);
			bufferPos.SetData (pos);
		}
		if (cloud2 != null){
			ReleaseBuffers2 ();
			bufferPoints2 = new ComputeBuffer (nCloud2, 12);
			bufferPoints2.SetData (cloud2);
			material2.SetBuffer ("buf_Points", bufferPoints2);
			Vector3[] pos2 = new Vector3[1];
			pos2[0] = new Vector3(0.0f, 0, 0);
			bufferPos2 = new ComputeBuffer (1, 12);
			material2.SetBuffer ("buf_Positions", bufferPos2);
			bufferPos2.SetData (pos2);
		}
	}
	
	private void ReleaseBuffers() 
	{
		if (bufferPoints != null)
			bufferPoints.Release();
		bufferPoints = null;
		if (bufferPos != null)
			bufferPos.Release();
		bufferPos = null;
	}
	
	
	private void ReleaseBuffers2() 
	{
		if (bufferPoints2 != null)
			bufferPoints2.Release();
		bufferPoints2 = null;
		if (bufferPos2 != null)
			bufferPos2.Release();
		bufferPos2 = null;
	}
	
	void OnDisable() 
	{
		ReleaseBuffers ();
	}
	
	void OnRenderObject () 
	{
		//int nCloud = cloudActive ? nCloud1 : nCloud2;
		material.SetPass (0);
		Graphics.DrawProcedural (MeshTopology.Points , nCloud1, 1);
		material2.SetPass (0);
		Graphics.DrawProcedural (MeshTopology.Points , nCloud2, 1);
	}
	
	private int loadCloud(string name, ref Vector3[] points) {
		if (!File.Exists(name)) {
			points = null;
			return 0;
		}
		string text = System.IO.File.ReadAllText(name);
		string[] lines = text.Split('\n');
		if (lines == null) {
			points = null;
			return 0;
		}
		int nLines = lines.Length;
		points = new Vector3[nLines];
		for (var i = 0; i < nLines; i++)
		{
			string[] point = lines[i].Split(' ');
			if (point == null) {
				continue;
			}
			if (point[0] != "nan" && point[0] != "") {
				points[i] = new Vector3(float.Parse(point[0]), float.Parse(point[1]), float.Parse(point[2]));
			} else {
				points[i] = new Vector3(0.0f, 0.0f, 0.0f);
			}
		}
		return nLines;
	}
	
	private void saveCloud(string name, Vector3[] cloud, int nCloud) {
		StreamWriter sw  = new StreamWriter(name);
		for (int i = 0; i < nCloud; i++) {
			sw.WriteLine(cloud[i].x + " " + cloud[i].y + " " + cloud[i].z);
		}
		sw.Close();
	}
	
	private void Solve(double[,] V, double[,] T, double[] R, ref double[] S) {
		double alfa = -((-V[1,2] * V[2,1] * R[0] + V[1,1] * V[2,2] * R[0] + V[1,2] * V[2,0] * R[1] - V[1,0] * V[2,2] * R[1] - V[1,1] * V[2,0] * R[2] + V[1,0] * V[2,1] * R[2]) /
		                (V[0,2] * V[1,1] * V[2,0] - V[0,1] * V[1,2] * V[2,0] - V[0,2] * V[1,0] * V[2,1] + V[0,0] * V[1,2] * V[2,1] + V[0,1] * V[1,0] * V[2,2] - V[0,0] * V[1,1] * V[2,2]));
		
		double beta = -((V[0, 2] * V[2, 1] * R[0] - V[0, 1] * V[2, 2] * R[0] - V[0, 2] * V[2, 0] * R[1] + V[0, 0] * V[2, 2] * R[1] + V[0, 1] * V[2, 0] * R[2] - V[0, 0] * V[2, 1] * R[2]) /
		                (V[0, 2] * V[1, 1] * V[2, 0] - V[0, 1] * V[1, 2] * V[2, 0] - V[0, 2] * V[1, 0] * V[2, 1] + V[0, 0] * V[1, 2] * V[2, 1] + V[0, 1] * V[1, 0] * V[2, 2] - V[0, 0] * V[1, 1] * V[2, 2]));
		
		double gamma = -((-V[0, 2] * V[1, 1] * R[0] + V[0, 1] * V[1, 2] * R[0] + V[0, 2] * V[1, 0] * R[1] - V[0, 0] * V[1, 2] * R[1] - V[0, 1] * V[1, 0] * R[2] + V[0, 0] * V[1, 1] * R[2]) /
		                 (V[0, 2] * V[1, 1] * V[2, 0] - V[0, 1] * V[1, 2] * V[2, 0] - V[0, 2] * V[1, 0] * V[2, 1] + V[0, 0] * V[1, 2] * V[2, 1] + V[0, 1] * V[1, 0] * V[2, 2] - V[0, 0] * V[1, 1] * V[2, 2]));
		
		S[0] = alfa * T[0, 0] + beta * T[1, 0] + gamma * T[2, 0];
		S[1] = alfa * T[0, 1] + beta * T[1, 1] + gamma * T[2, 1];
		S[2] = alfa * T[0, 2] + beta * T[1, 2] + gamma * T[2, 2];
	}
	
	private void updateCentreCloud() {
		Vector3[] cloud = cloudActive ? cloud1 : cloud2;
		int nCloud = cloudActive ? nCloud1 : nCloud2;
		float xMin = float.MaxValue, xMax = float.MinValue;
		float yMin = float.MaxValue, yMax = float.MinValue;
		float zMin = float.MaxValue, zMax = float.MinValue;
		for (int i = 0; i < nCloud; i++) {
			xMin = xMin > cloud[i].x ? cloud[i].x : xMin;
			yMin = yMin > cloud[i].y ? cloud[i].y : yMin;
			zMin = zMin > cloud[i].z ? cloud[i].z : zMin;
			xMax = xMax < cloud[i].x ? cloud[i].x : xMax;
			yMax = yMax < cloud[i].y ? cloud[i].y : yMax;
			zMax = zMax < cloud[i].z ? cloud[i].z : zMax;
		}
		rotateCentre = new Vector3 (xMin + ((xMax - xMin) / 2), yMin + ((yMax - yMin) / 2), zMin + ((zMax - zMin) / 2));
		inicializeMatrix();
	}
	
	private void saveMatrix(string name, Matrix4x4 matrix){
		StreamWriter sw  = new StreamWriter(name);
		sw.WriteLine(matrix[0,0] + " " + matrix[0,1] + " " +  matrix[0,2] + " " +  matrix[0,3]);
		sw.WriteLine(matrix[1,0] + " " + matrix[1,1] + " " +  matrix[1,2] + " " +  matrix[1,3]);
		sw.WriteLine(matrix[2,0] + " " + matrix[2,1] + " " +  matrix[2,2] + " " +  matrix[2,3]);
		sw.WriteLine(matrix[3,0] + " " + matrix[3,1] + " " +  matrix[3,2] + " " +  matrix[3,3]);
		sw.Close();
	}
	
	private void loadMatrix(string name, ref Matrix4x4 matrix) {
		if (!File.Exists(name)) {
			matrix = Matrix4x4.identity;
		}
		string text = System.IO.File.ReadAllText(name);
		string[] lines = text.Split('\n');
		if (lines == null) {
			matrix = Matrix4x4.identity;
		}
		matrix = Matrix4x4.identity;
		for (var i = 0; i < 4; i++)
		{
			string[] point = lines[i].Split(' ');
			if (point == null) {
				continue;
			}
			Debug.Log(lines[i]);
			matrix[i,0] = float.Parse(point[0]);
			matrix[i,1] = float.Parse(point[1]);
			matrix[i,2] = float.Parse(point[2]);
			matrix[i,3] = float.Parse(point[3]);
		}
	}
	private Vector3 transformPoint(Vector3 point, Matrix4x4 transform){
		Vector4 v = new Vector4(point.x, point.y, point.z, 1);
		v = transform * v;
		return new Vector3(v.x, v.y, v.z);
	}	
	private void transformCloud(Vector3[] cloud, int nCloud, Matrix4x4 matrix) {
		for (var i = 0; i < nCloud; i++) {
			//Vector4 v = new Vector4(cloud[i].x, cloud[i].y, cloud[i].z, 1);
			//v = matrix * v;
			
			//cloud[i] = new Vector3(v.x, v.y, v.z);
			cloud[i] = transformPoint(cloud[i], matrix);
		}
	}
	
	private void transformAllMatrix(Matrix4x4 transform){
		for(int i = 0; i < listMatrix.Count ; i++){
			//Debug.Log("Antes");
			//Debug.Log(listMatrix[i]);
			listMatrix[i] = transform * listMatrix[i];
			//Debug.Log("Despues");
			//Debug.Log(listMatrix[i]);
		}
	}
	
	private void saveAllMatrix(){
		for(int i = 0; i < listMatrix.Count ; i++){
			saveMatrix(string.Format("matrix{0}.txt", i), listMatrix[i]);
		}
	}
	
	private Matrix4x4 copyMatrix(Matrix4x4 matrixIN){
		Matrix4x4 matrixOUT = Matrix4x4.identity;
		matrixOUT[0,0] = matrixIN[0,0];
		matrixOUT[0,1] = matrixIN[0,1];
		matrixOUT[0,2] = matrixIN[0,2];
		matrixOUT[0,3] = matrixIN[0,3];
		matrixOUT[1,0] = matrixIN[1,0];
		matrixOUT[1,1] = matrixIN[1,1];
		matrixOUT[1,2] = matrixIN[1,2];
		matrixOUT[1,3] = matrixIN[1,3];
		matrixOUT[2,0] = matrixIN[2,0];
		matrixOUT[2,1] = matrixIN[2,1];
		matrixOUT[2,2] = matrixIN[2,2];
		matrixOUT[2,3] = matrixIN[2,3];
		matrixOUT[3,0] = matrixIN[3,0];
		matrixOUT[3,1] = matrixIN[3,1];
		matrixOUT[3,2] = matrixIN[3,2];
		matrixOUT[3,3] = matrixIN[3,3];
		return matrixOUT;
	}
	
	void Update()
	{
		Vector3[] cloud = cloudActive ? cloud1 : cloud2;
		int nCloud = cloudActive ? nCloud1 : nCloud2;
		Matrix4x4 transformMatrix = cloudActive ? transformMatrix1 : transformMatrix2;
		
		Vector3[] cloudAux = !cloudActive ? cloud1 : cloud2;
		int nCloudAux = !cloudActive ? nCloud1 : nCloud2;
		Matrix4x4 transformMatrixAux = !cloudActive ? transformMatrix1 : transformMatrix2;
		
		if(Input.GetKeyDown(KeyCode.P)) {
			//Debug.Log(transformMatrix1);
			
			Matrix4x4 pushMatrix = new Matrix4x4();
			pushMatrix = copyMatrix(transformMatrix1);
			listMatrix.Add(pushMatrix);
			
			cloud1 = cloud2;
			nCloud1 = nCloud2;
			//Debug.Log("Matrix 2");
			//Debug.Log(transformMatrix2);
			transformMatrix1 = copyMatrix(transformMatrix2);
			//Debug.Log(transformMatrix1);
			
			if (cloudActive){
				transformMatrix = copyMatrix(transformMatrixAux);
				transformMatrixAux = Matrix4x4.identity;
			}
			else{
				transformMatrixAux = copyMatrix(transformMatrix);
				transformMatrix = Matrix4x4.identity;
			}
			
			//transformMatrix1 = transformMatrix2;
			//Debug.Log("Matrix 1");
			//Debug.Log(transformMatrix1);
			transformMatrix2 = Matrix4x4.identity;
			nCloud2 = loadCloud (string.Format("cloud{0}.xyz", positionCloud++), ref cloud2);
			rePaint();
			message.text = "Saved";
			//Debug.Log("   ");
			updateCentreCloud();
			foreach(Matrix4x4 matrix in listMatrix)
				Debug.Log(matrix);
		}
		if(Input.GetKeyDown(KeyCode.M)) {
			saveCloud("nubeTransformada1.xyz",cloud1,nCloud1);
			saveCloud("nubeTransformada2.xyz",cloud2,nCloud2);
			Debug.Log("Cloud save");
		}
		if(Input.GetKeyDown(KeyCode.T)) {
			nCloud1 = loadCloud (string.Format("transformed{0}.xyz", 0), ref cloud1);
			nCloud2 = loadCloud (string.Format("transformed{0}.xyz", 1), ref cloud2);
			rePaint();
		}
		if(Input.GetKeyDown(KeyCode.R)) {
			saveAllMatrix();
			for (int i = 0; i < listMatrix.Count; i++) {
				nCloud = loadCloud (string.Format("cloud{0}.xyz", i), ref cloud);
				Matrix4x4 transform = Matrix4x4.identity;
				loadMatrix(string.Format("matrix{0}.txt", i), ref transform);
				transformCloud(cloud, nCloud, transform);
				saveCloud(string.Format("transformed{0}.xyz", i), cloud, nCloud);
				message.text = "Transforming cloud " + i;
			}
			//Application.Quit();
		}
		if(Input.GetKeyDown(KeyCode.C)) {
			cloudActive = !cloudActive;
			Debug.Log("Cloud: " + (cloudActive ? "Primera" : "Segunda"));
		}

		if(Input.GetKeyDown(KeyCode.KeypadPlus)) {
			moveSpeed = moveSpeed * 1.5f;
			angleSpeed = angleSpeed * 1.5f;
			inicializeMatrix();
			//Debug.Log(angleSpeed);
			//Debug.Log("Speed: +");
		}
		if(Input.GetKeyDown(KeyCode.KeypadMinus)) {
			moveSpeed = moveSpeed / 1.5f;
			angleSpeed = angleSpeed / 1.5f;
			inicializeMatrix();
			//Debug.Log(angleSpeed);
			//Debug.Log("Speed: -");
			//Debug.Log(transformMatrixW);
		}
		
		
		if (! Input.GetMouseButton(0)){
			if(Input.GetKey(KeyCode.RightArrow)) {
				for (int i = 0; i < nCloud; i++) {
					cloud[i] = new Vector3(cloud[i].x + moveSpeed, cloud[i].y, cloud[i].z);
				}
				if (Input.GetKey(KeyCode.LeftShift)) {
					for (int j = 0; j < nCloudAux; j++) {
						cloudAux[j] = new Vector3(cloudAux[j].x + moveSpeed, cloudAux[j].y, cloudAux[j].z);
					}
				}
				rotateCentre = new Vector3(rotateCentre.x + moveSpeed, rotateCentre.y, rotateCentre.z);
				inicializeMatrix();
				transformMatrix = transformMatrixR * transformMatrix;
				if (Input.GetKey(KeyCode.LeftShift)) {
					transformMatrixAux = transformMatrixR * transformMatrixAux;
					if (!cloudActive)
						transformAllMatrix(transformMatrixR);
				}
				if (cloudActive)
					transformAllMatrix(transformMatrixR);
				rePaint();		
				//Debug.Log(transformMatrix);
			}
			if(Input.GetKey(KeyCode.LeftArrow)) {
				for (int i = 0; i < nCloud; i++) {
					cloud[i] = new Vector3(cloud[i].x - moveSpeed, cloud[i].y, cloud[i].z);
				}
				if (Input.GetKey(KeyCode.LeftShift)) {
					for (int j = 0; j < nCloudAux; j++) {
						cloudAux[j] = new Vector3(cloudAux[j].x - moveSpeed, cloudAux[j].y, cloudAux[j].z);
					}
				}
				rotateCentre = new Vector3(rotateCentre.x - moveSpeed, rotateCentre.y, rotateCentre.z);
				inicializeMatrix();
				transformMatrix = transformMatrixL * transformMatrix;
				if (Input.GetKey(KeyCode.LeftShift)) {
					transformMatrixAux = transformMatrixL * transformMatrixAux;
					if (!cloudActive)
						transformAllMatrix(transformMatrixL);
				}
				if (cloudActive)
					transformAllMatrix(transformMatrixL);
				rePaint();
				//Debug.Log(transformMatrix);
			}
			if(Input.GetKey(KeyCode.UpArrow)) {
				for (int i = 0; i < nCloud; i++) {
					cloud[i] = new Vector3(cloud[i].x, cloud[i].y + moveSpeed, cloud[i].z);
				}
				if (Input.GetKey(KeyCode.LeftShift)) {
					for (int j = 0; j < nCloudAux; j++) {
						cloudAux[j] = new Vector3(cloudAux[j].x, cloudAux[j].y + moveSpeed, cloudAux[j].z);
					}
				}
				rotateCentre = new Vector3(rotateCentre.x, rotateCentre.y + moveSpeed, rotateCentre.z);
				inicializeMatrix();
				transformMatrix = transformMatrixU * transformMatrix;
				if (Input.GetKey(KeyCode.LeftShift)) {
					transformMatrixAux = transformMatrixU * transformMatrixAux;
					if (!cloudActive)
						transformAllMatrix(transformMatrixU);
				}
				if (cloudActive)
					transformAllMatrix(transformMatrixU);
				rePaint();
			}
			if(Input.GetKey(KeyCode.DownArrow)) {
				for (int i = 0; i < nCloud; i++) {
					cloud[i] = new Vector3(cloud[i].x, cloud[i].y - moveSpeed, cloud[i].z);
				}
				if (Input.GetKey(KeyCode.LeftShift)) {
					for (int j = 0; j < nCloudAux; j++) {
						cloudAux[j] = new Vector3(cloudAux[j].x, cloudAux[j].y - moveSpeed, cloudAux[j].z);
					}
				}
				rotateCentre = new Vector3(rotateCentre.x, rotateCentre.y - moveSpeed, rotateCentre.z);
				inicializeMatrix();
				transformMatrix = transformMatrixB * transformMatrix;
				if (Input.GetKey(KeyCode.LeftShift)) {
					transformMatrixAux = transformMatrixB * transformMatrixAux;
					if (!cloudActive)
						transformAllMatrix(transformMatrixB);
				}
				if (cloudActive)
					transformAllMatrix(transformMatrixB);
				rePaint();
			}
			if (Input.GetAxis("Mouse ScrollWheel") > 0) {
				for (int i = 0; i < nCloud; i++) {
					cloud[i] = new Vector3(cloud[i].x, cloud[i].y, cloud[i].z + moveSpeed*2);
				}
				if (Input.GetKey(KeyCode.LeftShift)) {
					for (int j = 0; j < nCloudAux; j++) {
						cloudAux[j] = new Vector3(cloudAux[j].x, cloudAux[j].y, cloudAux[j].z + moveSpeed*2);
					}
				}
				//Debug.Log("Punto pantalla: "+cloud[0]);
				rotateCentre = new Vector3(rotateCentre.x, rotateCentre.y, rotateCentre.z + moveSpeed*2);
				inicializeMatrix();
				transformMatrix = transformMatrixMF * transformMatrix;
				//Debug.Log("Punto transformado: "+transformMatrix*puntoUno);
				if (Input.GetKey(KeyCode.LeftShift)) {
					transformMatrixAux = transformMatrixMF * transformMatrixAux;
					if (!cloudActive)
						transformAllMatrix(transformMatrixMF);
				}
				if (cloudActive)
					transformAllMatrix(transformMatrixMF);
				rePaint();
			}
			if (Input.GetAxis("Mouse ScrollWheel") < 0) {
				for (int i = 0; i < nCloud; i++) {
					cloud[i] = new Vector3(cloud[i].x, cloud[i].y, cloud[i].z - moveSpeed*2);
				}
				if (Input.GetKey(KeyCode.LeftShift)) {
					for (int j = 0; j < nCloudAux; j++) {
						cloudAux[j] = new Vector3(cloudAux[j].x, cloudAux[j].y, cloudAux[j].z - moveSpeed*2);
					}
				}
				//Debug.Log("Punto pantalla: "+cloud[0]);
				rotateCentre = new Vector3(rotateCentre.x, rotateCentre.y, rotateCentre.z - moveSpeed*2);
				inicializeMatrix();
				transformMatrix = transformMatrixMB * transformMatrix;
				//Debug.Log("Punto transformado: "+transformMatrix*puntoUno);
				if (Input.GetKey(KeyCode.LeftShift)) {
					transformMatrixAux = transformMatrixMB * transformMatrixAux;
					if (!cloudActive)
						transformAllMatrix(transformMatrixMB);
				}
				if (cloudActive)
					transformAllMatrix(transformMatrixMB);
				rePaint();
			}
			
			
		}
		
		
		
		float mouseX = Input.GetAxis("Mouse X");
		float mouseY = Input.GetAxis("Mouse Y");
		//Debug.Log("Mouse X "+mouseX+" Mouse Y "+mouseY);
		
		if((mouseY > 0 && Input.GetMouseButton(0)) || Input.GetKey(KeyCode.W)) {
			for (int i = 0; i < nCloud; i++) {
				cloud[i] = transformPoint(cloud[i],transformMatrixW);
			}
			if (Input.GetKey(KeyCode.LeftShift)) {
				for (int j = 0; j < nCloudAux; j++) {
					cloudAux[j] = transformPoint(cloudAux[j],transformMatrixW);
				}
			}
			//Debug.Log("Punto pantalla: "+cloud[0]);
			transformMatrix = transformMatrixW * transformMatrix;
			//Debug.Log("Punto transformado: "+transformMatrix*puntoUno);
			if (Input.GetKey(KeyCode.LeftShift)) {
				transformMatrixAux = transformMatrixW * transformMatrixAux;
				if (!cloudActive)
					transformAllMatrix(transformMatrixW);
			}
			if (cloudActive)
				transformAllMatrix(transformMatrixW);
			rePaint();
			//Debug.Log(transformMatrix);
		}
		if((mouseY < 0 && Input.GetMouseButton(0)) || Input.GetKey(KeyCode.S)) {
			for (int i = 0; i < nCloud; i++) {
				cloud[i] = transformPoint(cloud[i],transformMatrixS);// transformMatrixS*cloud[i];
			}
			if (Input.GetKey(KeyCode.LeftShift)) {
				for (int j = 0; j < nCloudAux; j++) {
					cloudAux[j] = transformPoint(cloudAux[j],transformMatrixS);
				}
			}
			//Debug.Log("Punto pantalla: "+cloud[0]);
			//Debug.Log("Matrix: "+transformMatrix);
			//Debug.Log("Matrix S: "+transformMatrixS);
			transformMatrix = transformMatrixS * transformMatrix;
			//Debug.Log("Punto transformado: "+transformMatrix*puntoUno);
			if (Input.GetKey(KeyCode.LeftShift)) {
				transformMatrixAux = transformMatrixS * transformMatrixAux;
				if (!cloudActive)
					transformAllMatrix(transformMatrixS);
			}
			if (cloudActive)
				transformAllMatrix(transformMatrixS);
			rePaint();
			//Debug.Log(transformMatrix);
		}
		if((mouseX > 0 && Input.GetMouseButton(0)) || Input.GetKey(KeyCode.D)) {
			//updateCentreCloud ();
			for (int i = 0; i < nCloud; i++) {
				cloud[i] = transformPoint(cloud[i],transformMatrixD);
			}
			if (Input.GetKey(KeyCode.LeftShift)) {
				for (int j = 0; j < nCloudAux; j++) {
					cloudAux[j] = transformPoint(cloudAux[j],transformMatrixD);
				}
			}
			transformMatrix = transformMatrixD * transformMatrix;
			if (Input.GetKey(KeyCode.LeftShift)) {
				transformMatrixAux = transformMatrixD * transformMatrixAux;
				if (!cloudActive)
					transformAllMatrix(transformMatrixD);
			}
			if (cloudActive)
				transformAllMatrix(transformMatrixD);
			rePaint();
			//Debug.Log(transformMatrix);
		}
		if((mouseX < 0 && Input.GetMouseButton(0)) || Input.GetKey(KeyCode.A)) {
			for (int i = 0; i < nCloud; i++) {
				cloud[i] = transformPoint(cloud[i],transformMatrixA);
			}
			if (Input.GetKey(KeyCode.LeftShift)) {
				for (int j = 0; j < nCloudAux; j++) {
					cloudAux[j] = transformPoint(cloudAux[j],transformMatrixA);
				}
			}
			//updateCentreCloud ();
			transformMatrix = transformMatrixA * transformMatrix;
			if (Input.GetKey(KeyCode.LeftShift)) {
				transformMatrixAux = transformMatrixA * transformMatrixAux;
				if (!cloudActive)
					transformAllMatrix(transformMatrixA);
			}
			if (cloudActive)
				transformAllMatrix(transformMatrixA);
			rePaint();
			//Debug.Log(transformMatrix);
		}
		if ((Input.GetAxis("Mouse ScrollWheel") > 0 && Input.GetMouseButton(0)) || Input.GetKey(KeyCode.E)) {
			//updateCentreCloud ();
			for (int i = 0; i < nCloud; i++) {
				cloud[i] = transformPoint(cloud[i],transformMatrixE);
			}
			if (Input.GetKey(KeyCode.LeftShift)) {
				for (int j = 0; j < nCloudAux; j++) {
					cloudAux[j] = transformPoint(cloudAux[j],transformMatrixE);
				}
			}
			//updateCentreCloud ();
			//Debug.Log("Punto pantalla: "+cloud[0]);
			transformMatrix = transformMatrixE * transformMatrix;
			//Debug.Log("Punto transformado: "+transformMatrix*puntoUno);
			if (Input.GetKey(KeyCode.LeftShift)) {
				transformMatrixAux = transformMatrixE * transformMatrixAux;
				if (!cloudActive)
					transformAllMatrix(transformMatrixE);
			}
			if (cloudActive)
				transformAllMatrix(transformMatrixE);
			rePaint();
			//Debug.Log(transformMatrix);
		}
		if ((Input.GetAxis("Mouse ScrollWheel") < 0 && Input.GetMouseButton(0)) || Input.GetKey(KeyCode.Q)) {
			//updateCentreCloud ();
			for (int i = 0; i < nCloud; i++) {
				cloud[i] = transformPoint(cloud[i],transformMatrixQ);
			}
			if (Input.GetKey(KeyCode.LeftShift)) {
				for (int j = 0; j < nCloudAux; j++) {
					cloudAux[j] = transformPoint(cloudAux[j],transformMatrixQ);
				}
			}
			//Debug.Log("Punto pantalla: "+cloud[0]);
			//Debug.Log("Matrix: "+transformMatrix);
			//Debug.Log("Matrix Q: "+transformMatrixQ);
			transformMatrix = transformMatrixQ * transformMatrix;
			//Debug.Log("Cuenta a mano1: "+(transformMatrixQ*transformMatrixS)*puntoUno);
			//Debug.Log("Cuenta a mano2: "+(transformMatrixQ*(transformMatrixS*puntoUno)));
			//Debug.Log("Punto transformado: "+transformMatrix*puntoUno);
			if (Input.GetKey(KeyCode.LeftShift)) {
				transformMatrixAux = transformMatrixQ * transformMatrixAux;
				if (!cloudActive)
					transformAllMatrix(transformMatrixQ);
			}
			if (cloudActive)
				transformAllMatrix(transformMatrixQ);
			rePaint();
		}
		
		
		
		
		
		
		
		if (cloudActive){
			transformMatrix1 = transformMatrix;
		}
		else{
			transformMatrix2 = transformMatrix;
		}
		if (Input.GetKey(KeyCode.LeftShift)){
			if (!cloudActive){
				transformMatrix1 = transformMatrixAux;
			}
			else{
				transformMatrix2 = transformMatrixAux;
			}
		}
		
	}
}