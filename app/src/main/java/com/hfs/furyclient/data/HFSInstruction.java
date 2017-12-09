package com.hfs.furyclient.data;

import android.os.Parcel;
import android.os.Parcelable;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by hfs on 30.11.2017.
 */

public class HFSInstruction implements Parcelable {
    public static final Creator<HFSInstruction> CREATOR = new Creator<HFSInstruction>() {
        @Override
        public HFSInstruction createFromParcel(Parcel in) {
            return new HFSInstruction(in);
        }

        @Override
        public HFSInstruction[] newArray(int size) {
            return new HFSInstruction[size];
        }
    };
    /**
     * The instruction name.
     */
    public String name;
    /**
     * The instruction steps.
     */
    public List<Step> steps;
    /**
     * The current instruction step.
     */
    private int currentStepId = 0;

    private HFSInstruction(Parcel in) {
        name = in.readString();
        steps = new ArrayList<Step>();
        in.readTypedList(steps, Step.CREATOR);
        currentStepId = in.readInt();
    }

    /**
     * Returns the current step identifier.
     *
     * @return
     */
    public int getCurrentStepId() {
        return currentStepId;
    }

    /**
     * Sets the current step identifier.
     *
     * @param step
     */
    public void setCurrentStepId(int step) {
        currentStepId = step;
    }

    /**
     * Returns the current step.
     *
     * @return
     */
    public Step getCurrentStep() {
        return steps.get(currentStepId);
    }

    /**
     * Returns the steps count.
     *
     * @return
     */
    public int getStepsCount() {
        if (steps != null) {
            return steps.size();
        }
        return 0;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel parcel, int i) {
        parcel.writeString(name);
        parcel.writeTypedList(steps);
        parcel.writeInt(currentStepId);
    }

    /**
     * Instruction step.
     */
    public static class Step implements Parcelable {
        public static final Creator<Step> CREATOR = new Creator<Step>() {
            @Override
            public Step createFromParcel(Parcel in) {
                return new Step(in);
            }

            @Override
            public Step[] newArray(int size) {
                return new Step[size];
            }
        };
        /**
         * The step identifier.
         */
        public int id;
        /**
         * The step hint.
         */
        public String hint;
        public ArrayList<String> details;

        public Step(Parcel parcel) {
            id = parcel.readInt();
            hint = parcel.readString();
            details = parcel.createStringArrayList();
        }

        @Override
        public int describeContents() {
            return 0;
        }

        @Override
        public void writeToParcel(Parcel parcel, int i) {
            parcel.writeInt(id);
            parcel.writeString(hint);
            parcel.writeStringList(details);
        }
    }
}
