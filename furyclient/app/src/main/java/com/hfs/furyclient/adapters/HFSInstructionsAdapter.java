package com.hfs.furyclient.adapters;

import android.content.Context;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import com.hfs.furyclient.R;
import com.hfs.furyclient.data.HFSInstruction;

import java.util.List;

/**
 * Created by hfs on 01.12.2017.
 */

public class HFSInstructionsAdapter extends ArrayAdapter<HFSInstruction> {
    private Context mContext;
    private List<HFSInstruction> mInstructions;

    public HFSInstructionsAdapter(@NonNull Context context, int resource, @NonNull List<HFSInstruction> objects) {
        super(context, resource, objects);
        mContext = context;
        mInstructions = objects;
    }

    @NonNull
    @Override
    public View getView(int position, @Nullable View convertView, @NonNull ViewGroup parent) {
        ViewHolder viewHolder = null;
        if (convertView == null) {
            convertView = LayoutInflater.from(mContext).inflate(
                    R.layout.instruction_item, parent, false);
            viewHolder = new ViewHolder();
            viewHolder.name = convertView.findViewById(R.id.instr_name);
            convertView.setTag(viewHolder);
        } else {
            viewHolder = (ViewHolder) convertView.getTag();
        }

        HFSInstruction instruction = mInstructions.get(position);
        viewHolder.name.setText(instruction.name);
        boolean enabled = instruction.steps != null &&
                instruction.steps.size() > 0;
        viewHolder.name.setEnabled(enabled);
        viewHolder.name.setClickable(!enabled);

        return convertView;
    }

    /**
     * View holder.
     */
    private static class ViewHolder {
        public TextView name;
    }
}
