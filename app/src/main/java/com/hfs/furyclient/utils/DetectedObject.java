package com.hfs.furyclient.utils;

/**
 * Created by Александра on 07.12.2017.
 */

public class DetectedObject {

    // region Pivate members.
    private double x;
    private double y;
    private double w;
    private double h;
    private String className;
    // endregion

    // region Properties

    public void setClassName(String value) {
        this.className = value;
    }

    public String getClassName() {
        return className;
    }

    public double getX() { return x; }

    public void setX(double x) {
        this.x = x;
    }

    public double getY() {
        return y;
    }

    public void setY(double y) {
        this.y = y;
    }

    public double getW() {
        return w;
    }

    public void setW(double w) {
        this.w = w;
    }

    public double getH() {
        return h;
    }

    public void setH(double h) {
        this.h = h;
    }
    // endregion

    // region Constructor
    public DetectedObject (String name, double y, double x, double width, double height)
    {
        this.className = name;
        this.y = y;
        this.x = x;
        this.w = width;
        this.h = height;
    }
    //endregion
}
