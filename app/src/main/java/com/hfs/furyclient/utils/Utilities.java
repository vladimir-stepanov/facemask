package com.hfs.furyclient.utils;

import com.google.gson.JsonElement;
import com.google.gson.JsonParser;

import org.json.JSONArray;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by Александра on 07.12.2017.
 */

public final class Utilities {
    // JSON attrs
    public static final String DETECTED_OBJECTS = "detected_objects";

    public static final String CLASS_NAME = "class_name";
    public static final String BAR = "bar";
    public static final String PIN = "pin";
    public static final String BASE_TOP = "base_top";
    public static final String REGULATOR = "regulator";
    public static final String BASE_BOTTOM = "base_bottom";
    public static final String PLAFOND = "plafond";
    public static final String JOINT_MIDDLE = "joint_middle";
    public static final String JOINT_TOP = "joint_top";

    public static final String BOUNDING_BOX = "bounding_box";
    public static final String Y = "y";
    public static final String X = "x";
    public static final String W = "w";
    public static final String H = "h";

    public static ArrayList<DetectedObject> getObjectsFromJSON (String jsonString)
    {
        ArrayList<DetectedObject> result = new ArrayList<DetectedObject>();
        try {
            JSONObject detectedObj = new JSONObject(jsonString);
            JSONArray detectedArray = (JSONArray) detectedObj.get(DETECTED_OBJECTS);
            for (int i =0; i < detectedArray.length(); i++) {
                JSONObject obj = detectedArray.getJSONObject(i);
                String name = obj.getString(CLASS_NAME);
                JSONObject entity = obj.getJSONObject(BOUNDING_BOX);
                Double y = entity.getDouble(Y);
                Double x = entity.getDouble(X);
                Double w = entity.getDouble(W);
                Double h = entity.getDouble(H);
                result.add(new DetectedObject(name, y, x, w, h));
            }
        } catch (Exception ex) {
            System.out.println(ex);
        }

        return result;
    }





}
