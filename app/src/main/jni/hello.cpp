#include "com_glitter_ndktest_JNIUtils.h"
#include <common.h>
#include <search.h>
#include <pthread.h>
#include <unistd.h>


char *jstringToNative(JNIEnv *pEnv, jstring pJstring);

jstring nativeTojsString(JNIEnv *pEnv, char *string);

extern "C" JNIEXPORT jstring JNICALL
Java_com_glitter_ndktest_JNIUtils_sayHello
        (JNIEnv *env, jclass js, jstring jstring1) {
    char const *c_str = NULL;
    jboolean isCopy;
    c_str = env->GetStringUTFChars(jstring1, &isCopy);
    printf("isCopy:%d", isCopy);
    if (c_str == NULL) {
        return NULL;
    }
    LOGE("The String from Java : %s\n", c_str);
    env->ReleaseStringUTFChars(jstring1, c_str);
    char buff[128] = "Hello Im From JNI!";

    return env->NewStringUTF(buff);
}

char *jstringToNative(JNIEnv *env, jstring jstr) {
    if (jstr == NULL || env->ExceptionCheck() == JNI_TRUE) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        return NULL;
    }
    jbyteArray bytes = 0;
    jthrowable exc;
    char *result = 0;
    if (env->EnsureLocalCapacity(2) < 0) {
        return 0;
    }
    jclass jcls_str = env->FindClass("java/lang/String");
    jmethodID MID_STRING_getbytes = env->GetMethodID(jcls_str, "getBytes", "()[B");
    bytes = (jbyteArray) (env->CallObjectMethod(jstr, MID_STRING_getbytes));
    exc = env->ExceptionOccurred();
    if (!exc) {
        jint len = env->GetArrayLength(bytes);
        result = (char *) malloc(len + 1);
        if (result == 0) {
            env->DeleteLocalRef(bytes);
            return 0;
        }
        env->GetByteArrayRegion(bytes, 0, len, (jbyte *) (result));
        result[len] = 0;
    } else {
        env->DeleteLocalRef(exc);
    }
    env->DeleteLocalRef(exc);
    return (char *) result;
}
/**
 * 操作数组
 */
extern "C"
JNIEXPORT jintArray JNICALL
Java_com_glitter_ndktest_JNIUtils_opearteIntArray(JNIEnv *env, jobject object,
                                                  jintArray intArray_in) {
    // TODO: implement opearteIntArray()
    jint *intArray;
    intArray = env->GetIntArrayElements(intArray_in, NULL);
    if (intArray == NULL) {
        return NULL;
    }
    const int length = env->GetArrayLength(intArray_in);
    LOGE("The intArray length is:%d\n", length);
    for (int i = 0; i < length; ++i) {
        LOGE("The intArray_in[%d] is %d\n", i, intArray[i]);
    }

    if (length > 0) {
        jint *buf_in;
        buf_in = (int *) malloc(sizeof(int) * length);
        env->GetIntArrayRegion(intArray_in, 0, length, buf_in);
        free(buf_in);
    }
    env->ReleaseIntArrayElements(intArray_in, intArray, 0);

    jintArray intArray_out;
    const int len_out = 10;
    intArray_out = env->NewIntArray(len_out);
    jint buf_out[len_out] = {0};
    for (int i = 0; i < len_out; ++i) {
        buf_out[i] = i * 2;
    }
    env->SetIntArrayRegion(intArray_out, 0, len_out, buf_out);
    return intArray_out;
}

jstring nativeTojsString(JNIEnv *env, char *string) {
    jclass jsclass = env->FindClass("java/lang/String");
    jmethodID id = env->GetMethodID(jsclass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes = env->NewByteArray((jsize) (strlen(string)));
    env->SetByteArrayRegion(bytes, 0, (jsize) strlen(string), (jbyte *) string);
    jstring encoding = env->NewStringUTF("utf-8");
    return (jstring) env->NewObject(jsclass, id, bytes, encoding);
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_glitter_ndktest_JNIUtils_operateStringArray(JNIEnv *env, jobject thiz,
                                                     jobjectArray array) {
    // TODO: implement operateStringArray()
    jsize size = env->GetArrayLength(array);
    for (int i = 0; i < size; ++i) {
        jstring string_in = (jstring) (env->GetObjectArrayElement(array, i));
        char *char_in = jstringToNative(env, string_in);
        LOGE("ObjectArrat_in [%d]:%s", i, char_in);
    }

    jclass clazz = env->FindClass("java/lang/String");
    jobjectArray objectArray_out;
    const int len_out = 5;
    objectArray_out = env->NewObjectArray(len_out, clazz, NULL);
    char *char_out[] = {"hello", "this", "is", "from", "jni"};
    jstring temp_string;
    for (int i = 0; i < len_out; ++i) {
        temp_string = nativeTojsString(env, char_out[i]);
        env->SetObjectArrayElement(objectArray_out, i, temp_string);
    }
    return objectArray_out;
}
typedef struct Person {
    char *mName;
    int mAge;
};

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_glitter_ndktest_JNIUtils_operatePersonArray(JNIEnv *env, jobject thiz,
                                                     jobjectArray array) {
    // TODO: implement operatePersonArray()
    jclass clazz = env->FindClass("com/glitter/ndktest/Person");
    jfieldID mAge = env->GetFieldID(clazz, "mAge", "I");
    jfieldID mName = env->GetFieldID(clazz, "mName", "Ljava/lang/String;");
    const int object_len = env->GetArrayLength(array);


    jobject mObject_in;
    Person person_in[object_len];
    for (int i = 0; i < object_len; ++i) {
        mObject_in = env->GetObjectArrayElement(array, i);
        person_in[i].mAge = env->GetIntField(mObject_in, mAge);
        person_in[i].mName = jstringToNative(env, (jstring) env->GetObjectField(mObject_in, mName));
        LOGE("mObject_in[%d].mAge : %d\n", i, person_in[i].mAge);
        LOGE("mObject_in[%d].mName : %s\n", i, person_in[i].mName);
    }
    const int object_num = 2;

    Person persons[object_num];
    for (int i = 0; i < object_len; ++i) {
        persons[i].mAge = 20 + i;
        persons[i].mName = (char *) malloc(sizeof(char) * 20);
        sprintf(persons[i].mName, "name:%d", i);
    }

    jmethodID initMethod = env->GetMethodID(clazz, "<init>", "()V");

    jobject mObject_out;
    jobjectArray mArray_out = env->NewObjectArray(object_num, clazz, NULL);
    for (int i = 0; i < object_num; ++i) {
        mObject_out = env->NewObject(clazz, initMethod);
        env->SetIntField(mObject_out, mAge, persons[i].mAge);
        env->SetObjectField(mObject_out, mName, nativeTojsString(env, persons[i].mName));
        env->SetObjectArrayElement(mArray_out, i, mObject_out);
    }
    return mArray_out;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_glitter_ndktest_JNIUtils_accessInstanceFiled(JNIEnv *env, jobject thiz, jobject test) {
    // TODO: implement accessInstanceFiled()
    jclass clazz;
    jfieldID mString_fieldID;
    jfieldID mInt_fieldID;

    clazz = env->GetObjectClass(test);

    if (clazz == NULL) {
        LOGE("GETobjectClass failed\n");
        return;
    }
    mString_fieldID = env->GetFieldID(clazz, "mString", "Ljava/lang/String;");
    if (mString_fieldID == NULL) {
        LOGE("GetFieldID mString failed\n");
        return;
    }
    jstring jstring1 = (jstring) env->GetObjectField(test, mString_fieldID);
    char *buf = jstringToNative(env, jstring1);
    free(buf);

    char *buf_out = "hello java,I am JNI!";
    env->SetObjectField(test, mString_fieldID, nativeTojsString(env, buf_out));

    env->DeleteLocalRef(jstring1);

    mInt_fieldID = env->GetFieldID(clazz, "mInt", "I");
    if (mInt_fieldID == NULL) {
        LOGE("GEtINt ERROR");
        return;
    }
    jint mInt = env->GetIntField(test, mInt_fieldID);
    LOGE("object_in.mInt:%d", mInt);
    env->SetIntField(test, mInt_fieldID, 190);
    env->DeleteLocalRef(clazz);

}extern "C"
JNIEXPORT void JNICALL
Java_com_glitter_ndktest_JNIUtils_accessStatic(JNIEnv *env, jobject thiz) {
    // TODO: implement accessStatic()
    jclass clazz;
    jfieldID mStaticString_fieldID;
    jfieldID mStaticInt_fieldId;

    clazz = env->FindClass("com/glitter/ndktest/Test");

    if (clazz == NULL) {
        LOGE("ERROR");
        return;
    }

    mStaticString_fieldID = env->GetStaticFieldID(clazz, "name", "Ljava/lang/String;");

    jstring j_string = (jstring) env->GetStaticObjectField(clazz, mStaticString_fieldID);
    char *buf = jstringToNative(env, j_string);
    LOGE("JNIFIELDClass.mString:%s\n", buf);
    free(buf);
    char *buf_out = const_cast<char *>("Hello I'mAlice");
    env->SetStaticObjectField(clazz, mStaticString_fieldID, nativeTojsString(env, buf_out));

    env->DeleteLocalRef(j_string);

    mStaticInt_fieldId = env->GetStaticFieldID(clazz, "age", "I");
    jint jint1 = env->GetStaticIntField(clazz, mStaticInt_fieldId);
    LOGE("this is from jni's age is %d", jint1);
    int const age = 100;
    env->SetStaticIntField(clazz, mStaticInt_fieldId, age);
    env->DeleteLocalRef(clazz);


}extern "C"
JNIEXPORT void JNICALL
Java_com_glitter_ndktest_JNIUtils_getValue(JNIEnv *env, jobject thiz) {
    // TODO: implement getValue()
    jclass clazz = env->FindClass("com/glitter/ndktest/Test");
    jmethodID jCon_ID;
    if (clazz == NULL
            ) {
        LOGE("Error");
        return;
    }
    jCon_ID = env->GetMethodID(clazz, "<init>", "()V");
    jobject jobject1 = env->NewObject(clazz, jCon_ID);
    jmethodID jniInstanceId = env->GetMethodID(clazz, "getValue", "(II)I");
    jint a = 4;
    jint b = 5;
    env->CallIntMethod(jobject1, jniInstanceId, a, b);
    env->DeleteLocalRef(clazz);
    env->DeleteLocalRef(jobject1);
}extern "C"
JNIEXPORT void JNICALL
Java_com_glitter_ndktest_JNIUtils_getStaticValue(JNIEnv *env, jobject thiz) {
//    // TODO: implement getStaticValue()
    jclass clazz = env->FindClass("com/glitter/ndktest/Test");
    jmethodID jmethodId = env->GetStaticMethodID(clazz, "sum", "(II)I");
    jint a = 4;
    jint b = 56;
    jint jint1 = env->CallStaticIntMethod(clazz, jmethodId, a, b);
    LOGE("this is jnit%d", jint1);

    jclass jclass1 = env->FindClass("com/glitter/ndktest/JNIUtils");
    jmethodID jmethodId1 = env->GetMethodID(jclass1, "getBytes", "()V");
    env->CallVoidMethod(thiz, jmethodId1);

    env->DeleteLocalRef(clazz);
}extern "C"
JNIEXPORT void JNICALL
Java_com_glitter_ndktest_JNIUtils_throwException(JNIEnv *env, jobject thiz) {
    // TODO: implement throwException()
    jthrowable mThrowable;
    jclass jclass1 = env->GetObjectClass(thiz);
    jmethodID jmethodId = env->GetMethodID(jclass1, "callBackException", "()V");
    if (jmethodId == NULL) {
        return;
    }
    env->CallVoidMethod(thiz, jmethodId);
    mThrowable = env->ExceptionOccurred();
    if (env->ExceptionCheck()) {
        LOGE("ExceptionCheck has exception\n");
    }
    if (mThrowable) {
        jclass newException;

        //7.打印异常信息堆栈
        env->ExceptionDescribe();
        //8.清除异常信息
        env->ExceptionClear();

        newException = env->FindClass("java/lang/IllegalArgumentException");
        if(NULL == newException)
        {
            //Unable to find the Class
            LOGE("FindClass failed\n");
            return;
        }

        //9.抛出一个自定义异常信息
        env->ThrowNew(newException, "Sorry Java, Create exception for you!");
    }
    env->DeleteLocalRef(mThrowable);
    env->DeleteLocalRef(jclass1);
}
JavaVM *gJavaVM = NULL;//全局JavaVM 变量
jobject gJavaObj = NULL;//全局Jobject变量

jmethodID nativeCallback = NULL;//全局的方法ID
static int count = 0;


static void* native_thread_exec(void *arg)
{
    LOGE("nativeThreadExec");
    LOGE("The pthread id : %d\n", pthread_self());
    JNIEnv *env;
    //从全局的JavaVM中获取到环境变量
    gJavaVM->AttachCurrentThread(&env,NULL);


    //线程循环
    for(int i = 0 ; i < 5; i++)
    {
        usleep(2);
        //跨线程回调Java层函数
        env->CallVoidMethod(gJavaObj,nativeCallback,count++);
    }
    gJavaVM->DetachCurrentThread();
    LOGE("thread stoped");
    return ((void *)0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_glitter_ndktest_JNIUtils_nativeInit(JNIEnv *env, jobject object) {
    // TODO: implement nativeInit()
    LOGE("Java_com_xxx_api_thread_NativeThread_nativeInit\n");

    gJavaObj = env->NewGlobalRef(object);//创建全局引用
    jclass clazz = env->GetObjectClass(object);
    nativeCallback = env->GetMethodID(clazz,"onNativeCallback","(I)V");


    //操作方式二，调用JNI函数保存JavaVM
    env->GetJavaVM(&gJavaVM);
    pthread_t id;
    //通过pthread库创建线程
    LOGE("create native thread\n");
    if(pthread_create(&id,NULL,native_thread_exec,NULL)!=0)
    {
        LOGE("native thread create fail");
        return;
    }
    for(int i = 0 ; i < 5; i++)
    {
        usleep(20);
        //跨线程回调Java层函数
        env->CallVoidMethod(gJavaObj,nativeCallback,count++);
    }

    LOGE("native thread creat success");

}
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    LOGE("JNI_OnLoad\n");
    JNIEnv* env = NULL;
    //获取JNI_VERSION版本
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        LOGE("checkversion error\n");
        return -1;
    }

    //操作方式一，通过SO加载时保存全局JavaVM
    //gJavaVM = vm;

    //返回jni 的版本
    return JNI_VERSION_1_6;
}