package com.example.myapplication;//***insert your package here***

/*
 * SpeechRepeatActivity
 * - demonstrate speech recognition and TTS repeat
 * - as outlined in Mobiletuts tutorial
 * - "Android SDK: Implementing Speech Recognition with a Speak and Repeat App"
 * 
 *  Sue Smith
 *  29.05.12
 */

import java.util.ArrayList;
import java.util.List;
import java.util.Locale; 

import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.os.Bundle;
import android.speech.RecognizerIntent;
import android.speech.tts.TextToSpeech.OnInitListener;
import android.speech.tts.TextToSpeech;   
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Toast;
import android.widget.TextView;

/**
 * SpeechRepeatActivity
 * - processes speech input 
 * - presents user with list of suggested words
 * - when user selects a word from the list, the app speaks the word back using the TTS engine 
 */
public class SpeechRepeatActivity extends Activity implements OnClickListener, OnInitListener {
	
	//переменная для проверки распознование голоса на этом устройстве
	private static final int VR_REQUEST = 999;

    // переменная для проверки данных движка TTS на пользовательском устройстве
    private int MY_DATA_CHECK_CODE = 0;
    
    //Text To Speech instance кароч, это переменная(или список) в которой находится то, что мы сказали. Можно будет сделать if губка и т.д.
    private TextToSpeech repeatTTS;


    // ListView для отображения предлагаемых слов
	private ListView wordList;


    // Тег журнала для выходной информации
	private final String LOG_TAG = "SpeechRepeatActivity";
	
    /**Создайте Activity, подготовьтесь к обработке речи и повторите
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {

        // вызов суперкласса

        super.onCreate(savedInstanceState);
        // устанавливаем просмотр содержимого
        setContentView(R.layout.activity_main);

        // получаем ссылку на кнопку разговора
        Button speechBtn = (Button) findViewById(R.id.speech_btn);
        // получаем ссылку на список слов
        wordList = (ListView) findViewById(R.id.word_list);

        // узнаем, поддерживается ли распознавание речи
        PackageManager packManager = getPackageManager();
        List<ResolveInfo> intActivities = packManager.queryIntentActivities
        		(new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH), 0);
        if (intActivities.size() != 0) {
            // распознавание речи поддерживается - обнаружение нажатий кнопок пользователем
            speechBtn.setOnClickListener(this);
            // готовим TTS к повторению выбранных слов
            Intent checkTTSIntent = new Intent();
            // проверяем данные TTS
            checkTTSIntent.setAction(TextToSpeech.Engine.ACTION_CHECK_TTS_DATA);
            // запускаем проверку Intent - получим результат в onActivityResult
            startActivityForResult(checkTTSIntent, MY_DATA_CHECK_CODE); 
        }
        else 
        {
            // распознавание речи не поддерживается, отключить кнопку и вывести сообщение
            speechBtn.setEnabled(false);
            Toast.makeText(this, "Oops - Speech recognition not supported!", Toast.LENGTH_LONG).show();
        }

        // обнаруживаем, что пользователь нажимает на предложенные слова
        wordList.setOnItemClickListener(new OnItemClickListener() {

            // щелкните прослушиватель для элементов в списке
        	public void onItemClick(AdapterView<?> parent, View view, int position, long id) 
            {
                // преобразуем представление
            	TextView wordView = (TextView)view;
                // получаем выбранное слово
            	String wordChosen = (String) wordView.getText();
                // вывод для отладки
            	Log.v(LOG_TAG, "chosen: "+wordChosen);
                // произносим слово с помощью TTS
            	repeatTTS.speak("You said: "+wordChosen, TextToSpeech.QUEUE_FLUSH, null);
                // выводим Toast-сообщение
            	Toast.makeText(SpeechRepeatActivity.this, "You said: "+wordChosen, Toast.LENGTH_SHORT).show();//**alter for your Activity name***
            }
        });
    }
    
    /**
     * Вызывается, когда пользователь нажимает кнопку разговора
     */
    public void onClick(View v) {
        if (v.getId() == R.id.speech_btn) {
            // ожидаем результатов
            listenToSpeech();
        }
    }
    
    /**
     * Укажите приложению, чтобы оно прислушивалось к речевому вводу пользователя
     */
    private void listenToSpeech() {

        // запускаем намерение распознавания речи, передавая необходимые данные
    	Intent listenIntent = new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH);
        // указываем пакет
    	listenIntent.putExtra(RecognizerIntent.EXTRA_CALLING_PACKAGE, getClass().getPackage().getName());
        // сообщение для отображения во время прослушивания
    	listenIntent.putExtra(RecognizerIntent.EXTRA_PROMPT, "Say a word!");
        // устанавливаем речевую модель
    	listenIntent.putExtra(RecognizerIntent.EXTRA_LANGUAGE_MODEL, RecognizerIntent.LANGUAGE_MODEL_FREE_FORM);
        // указываем количество результатов для получения
    	listenIntent.putExtra(RecognizerIntent.EXTRA_MAX_RESULTS, 10);

        // начинаем слушать
        startActivityForResult(listenIntent, VR_REQUEST);
    }
    
    /**
     * onActivityResults обрабатывает:
     * - получение результатов прослушивания распознавания речи
     * - получение результата проверки данных TTS
     */
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        // проверяем результат распознавания речи
        if (requestCode == VR_REQUEST && resultCode == RESULT_OK) 
        {
            // сохраняем возвращенный список слов как ArrayList
            ArrayList<String> suggestedWords = data.getStringArrayListExtra(RecognizerIntent.EXTRA_RESULTS);
            // устанавливаем полученный список для отображения в ListView с помощью ArrayAdapter
            wordList.setAdapter(new ArrayAdapter<String> (this, R.layout.word, suggestedWords));
        }

        // возвращено из проверки данных TTS
        if (requestCode == MY_DATA_CHECK_CODE) 
        {
            // у нас есть данные - создаем экземпляр TTS
	        if (resultCode == TextToSpeech.Engine.CHECK_VOICE_DATA_PASS)  
	        	repeatTTS = new TextToSpeech(this, this);
                // данные не установлены, предлагаем пользователю установить их
	        else 
	        {
                // намерение приведет пользователя на страницу загрузки TTS в Google Play
	        	Intent installTTSIntent = new Intent();  
	        	installTTSIntent.setAction(TextToSpeech.Engine.ACTION_INSTALL_TTS_DATA);  
	        	startActivity(installTTSIntent);  
	        }  
        }

        // вызов метода суперкласса
        super.onActivityResult(requestCode, resultCode, data);
    }
    
    /**
     * onInit срабатывает при инициализации TTS
     */
    public void onInit(int initStatus) {
        // в случае успеха устанавливаем локаль
    	 if (initStatus == TextToSpeech.SUCCESS)   
    	  repeatTTS.setLanguage(Locale.UK);//***выберите здесь свой собственный регион***
    	   
    }
}
