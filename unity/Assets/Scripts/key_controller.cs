using UnityEngine;
using System.Collections;
using System;
using System.IO;
using System.Text;

public class key_controller : MonoBehaviour
{
		private TextAsset text;
		private StringReader rs;
		private string line;
		private float length;//length of each note
		private float sound_length;//length for sound
		private float pre_time;//time when previous note read
		private float pre_sound_time;//time when previous note read
		private GameObject key;
		private GameObject light;
		
		private const double PI = System.Math.PI;
		private double gain = 1.5;
		private double sampling_frequency = 48000;
		private double phase;//phase of SineWave
		private double increment;//increment of phase
		private string playState = "no";
		private float envelope;
		private float fadeScale = 0.1f;
		private bool sound_flag = false;
		private bool key_flag = false;
		private float key_delay = 0.5f;//light_height / light_spd
		private float sound_delay = 0.9f;//key_depth / key_spd + key_delay
		
		private void SineWave (float[] data, int channels, double frequency)
		{
				increment = frequency * 2 * PI / sampling_frequency;
				for (var i = 0; i < data.Length; i = i + channels) {
						phase = phase + increment;
						data [i] = (float)(gain * Math.Sin (phase)) * envelope * fadeScale;
			
						if (fadeScale < 1.0f)
								fadeScale *= 1.5f; // avoid noise at start of sound 
									
						if (channels == 2)
								data [i + 1] = data [i];
						if (phase > 2 * Math.PI)
								phase = 0;
				}
		}
		
		void OnAudioFilterRead (float[] data, int channels)
		{
				switch (playState) {
				case "C4":
						SineWave (data, channels, 261.6255653005985);
						break;
				case "CS4":
						SineWave (data, channels, 277.18263097687196);
						break;
				case "D4":
						SineWave (data, channels, 293.66476791740746);
						break;
				case "DS4":
						SineWave (data, channels, 311.1269837220808);
						break;
				case "E4":
						SineWave (data, channels, 329.62755691286986);
						break;
				case "F4":
						SineWave (data, channels, 349.2282314330038);
						break;
				case "FS4":
						SineWave (data, channels, 369.99442271163434);
						break;
				case "G4":
						SineWave (data, channels, 391.99543598174927);
						break;
				case "GS4":
						SineWave (data, channels, 415.3046975799451);
						break;
				case "A4":
						SineWave (data, channels, 440.0);
						break;
				case "AS4":
						SineWave (data, channels, 466.1637615180899);
						break;
				case "H4":
						SineWave (data, channels, 493.8833012561241);
						break;
				case "C5":
						SineWave (data, channels, 261.6255653005985 * 2);
						break;
				case "CS5":
						SineWave (data, channels, 277.18263097687196 * 2);
						break;
				case "D5":
						SineWave (data, channels, 293.66476791740746 * 2);
						break;
				case "DS5":
						SineWave (data, channels, 311.1269837220808 * 2);
						break;
				case "E5":
						SineWave (data, channels, 329.62755691286986 * 2);
						break;
				case "F5":
						SineWave (data, channels, 349.2282314330038 * 2);
						break;
				case "FS5":
						SineWave (data, channels, 369.99442271163434 * 2);
						break;
				case "G5":
						SineWave (data, channels, 391.99543598174927 * 2);
						break;
				case "GS5":
						SineWave (data, channels, 415.3046975799451 * 2);
						break; 
				case "A5":
						SineWave (data, channels, 440.0 * 2);
						break;
				case "AS5":
						SineWave (data, channels, 466.1637615180899 * 2);
						break;
				case "H5":
						SineWave (data, channels, 493.8833012561241);
						break;
				}
		}
		
		void score_load (string score_name)
		{
				text = Resources.Load (score_name) as TextAsset;
				rs = new System.IO.StringReader (text.text);
		
		}
		
		// Use this for initialization
		void Start ()
		{	
				light = GameObject.Find ("Point light"); 
				score_load ("score");
		}
	
		// Update is called once per frame
		void Update ()
		{	

				if (pre_time + length < Time.time) {
						if (rs.Peek () > -1) {
								line = rs.ReadLine ();
								pre_time = Time.time;
								length = float.Parse (line);
								sound_flag = true;
								if (length > 0) {
										line = rs.ReadLine ();
										key = GameObject.Find ("key" + line);
										light.SendMessage ("Recieve", key);
										key_flag = true;
								} else {
										line = "no";
										length = -length;
								}
						} else {
								playState = "no";
								score_load ("score");
								pre_time = Time.time;
								length = 1;
								
						}
				}
				
				if (key_flag && pre_time + key_delay < Time.time) {
						key.SendMessage ("Recieve");
						key_flag = false;
				}
				
				if (sound_flag && pre_time + sound_delay < Time.time) {
						pre_sound_time = Time.time;
						sound_length = length;
						playState = line;
						phase = 0;
						fadeScale = 0.1f;
						sound_flag = false;

				}
				
				envelope = (pre_sound_time + sound_length - Time.time) / sound_length;
		}
	
	
}
