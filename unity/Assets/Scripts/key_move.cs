using UnityEngine;
using System.Collections;

public class key_move : MonoBehaviour
{
		private int key_status = 0;//0:default, 1:down, 2:pressed, 3:up
		private float press_time;//time when key pressed
		private float default_y;//default key position(y)
		private float key_spd = 1f;
		private float key_depth = 0.4f;
		private float def_brightness;
		private float brightness;
		private bool BRIGHT = true;

		public void Recieve ()
		{
				key_status = 1;
				press_time = Time.time;
		}

		// Use this for initialization
		void Start ()
		{
				default_y = this.transform.position.y;
				if (BRIGHT) {
						def_brightness = this.GetComponent<Renderer> ().material.color.r;
						brightness = def_brightness;
				}
		}
	
		// Update is called once per frame
		void Update ()
		{
				switch (key_status) {
				case 1:
						if (transform.position.y <= default_y - key_depth) {
								key_status = 2;
						} else {
								if (BRIGHT) {
										brightness += 0.02f;
										Color color = new Color (brightness, brightness, brightness, 1.0f);
										GetComponent<Renderer> ().material.color = color;
								}
								transform.Translate (Vector3.down * Time.deltaTime * key_spd);
						}
						break;
				case 2:
						if (press_time + 1f < Time.time) {
								key_status = 3;
						}
						break;
				case 3:
						if (transform.position.y >= default_y) {
								key_status = 0;
								if (BRIGHT) {
										brightness = def_brightness;
										Color color = new Color (brightness, brightness, brightness, 1.0f);
										GetComponent<Renderer> ().material.color = color;
								}
						} else {
								if (BRIGHT) {
										brightness -= 0.02f;
										Color color = new Color (brightness, brightness, brightness, 1.0f);
										GetComponent<Renderer> ().material.color = color;
								}							
								transform.Translate (Vector3.up * Time.deltaTime * key_spd);
						}
						break; 
				}
		}
}