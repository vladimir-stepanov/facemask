package com.hfs.furyclient.activities;

import android.content.Intent;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.google.gson.Gson;
import com.hfs.furyclient.R;
import com.hfs.furyclient.adapters.HFSInstructionsAdapter;
import com.hfs.furyclient.data.HFSInstruction;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

public class HFSMainActivity extends AppCompatActivity implements AdapterView.OnItemClickListener {
    private static final String TAG = HFSMainActivity.class.getSimpleName();
    private static final int REQUEST_CODE_START_INSTRUCTION = 1;
    private List<HFSInstruction> mInstructionsList;
    private ListView mInstructionsListView;
    private RelativeLayout mLoadingLayout;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        ActionBar bar = getSupportActionBar();
        if (bar != null) {
            bar.setTitle(null);
            View view = LayoutInflater.from(this).inflate(
                    R.layout.action_bar_title_view, null, false);
            TextView title = view.findViewById(R.id.title);
            title.setText(R.string.instructions_text);
            bar.setCustomView(view);
            bar.setDisplayShowCustomEnabled(true);
        }
        mInstructionsList = new ArrayList<>();
        mInstructionsListView = findViewById(R.id.instr_list_view);
        mLoadingLayout = findViewById(R.id.loading_layout);
        loadInstructions();
    }

    /**
     * Loads instructions.
     */
    private void loadInstructions() {
        showLoading(true);

        AssetManager assetManager = getAssets();
        try {
            String[] assets = assetManager.list("");
            InputStream inputStream = null;
            for (String asset : assets) {
                String ext = "";
                int i = asset.lastIndexOf(".");
                if (i > 0) {
                    ext = asset.substring(i + 1);
                    if (TextUtils.equals(ext, "json")) {
                        inputStream = assetManager.open(asset);
                        byte[] buffer = new byte[inputStream.available()];
                        inputStream.read(buffer);
                        HFSInstruction instruction = new Gson().fromJson(
                                new String(buffer, "UTF-8"), HFSInstruction.class);
                        if (instruction != null &&
                                !TextUtils.isEmpty(instruction.name)) {
                            mInstructionsList.add(instruction);
                        }
                    }
                }
            }
        } catch (IOException e) {
            Log.e(TAG, String.format(e.getMessage()));
        }
        setInstructionsList();
    }

    /**
     * Sets the instructions list.
     */
    private void setInstructionsList() {
        HFSInstructionsAdapter adapter = new HFSInstructionsAdapter(
                this, R.layout.instruction_item, mInstructionsList);
        if (mInstructionsListView != null) {
            mInstructionsListView.setAdapter(adapter);
            mInstructionsListView.setOnItemClickListener(this);
        }
        showLoading(false);
    }

    /**
     * Shows the loading layout.
     *
     * @param show
     */
    private void showLoading(boolean show) {
        if (mLoadingLayout != null) {
            mLoadingLayout.setVisibility(show ? View.VISIBLE : View.GONE);
        }
    }

    private void startInstruction(HFSInstruction instruction) {
        if (instruction != null) {
            Intent intent = new Intent("android.intent.action.HFS_INSTRUCTION_ACTIVITY");
            intent.putExtra(HFSInstructionActivity.EXTRA_HFS_INSTRUCTION, instruction);
            startActivityForResult(intent, REQUEST_CODE_START_INSTRUCTION);
        }
    }

    @Override
    public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
        if (mInstructionsList != null) {
            startInstruction(mInstructionsList.get(i));
        }
    }
}
