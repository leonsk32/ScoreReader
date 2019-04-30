using UnityEngine;
using System.Collections;

public class light_move : MonoBehaviour
{
		private int light_status = 0;//0:default, 1:down, 2:slow down, 3:up)
		private float light_spd = 10f;
		private float light_slow = 1f;
		private float light_col_y = 0;//slowdown height = key height
		private float light_dis_y = 0;//disappear height

		public void Recieve (GameObject key)
		{
				light_status = 1;
				GetComponent<Light>().intensity = 1.0f;
				light_dis_y = key.transform.position.y;
				light_col_y = light_dis_y + 0.5f;
				if (key.transform.position.y > 0.3f) {
						transform.position = key.transform.position + new Vector3 (1.0f, 5.5f, 0);
				} else {
						transform.position = key.transform.position + new Vector3 (2.0f, 5.5f, 0);
				}
		}

		// Use this for initialization
		void Start ()
		{
	
		}
	
		// Update is called once per frame
		void Update ()
		{
				switch (light_status) {
				case 1:
						if (transform.position.y <= light_col_y) {
								light_status = 2;
						} else {
								transform.Translate (Vector3.down * Time.deltaTime * light_spd); 
						}
						break;
				case 2:
						if (transform.position.y <= light_dis_y) {
								light_status = 3;
						} else {
								transform.Translate (Vector3.down * Time.deltaTime * light_slow);
								GetComponent<Light>().intensity -= 2f * Time.deltaTime;
						}
						break;
				case 3:
						GetComponent<Light>().intensity -= 2f * Time.deltaTime;
						break;
				}
		}
}
