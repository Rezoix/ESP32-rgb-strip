import React, { FunctionComponent, useEffect } from "react";
import { Theme, makeStyles, Grid } from "@material-ui/core";
import axios from "axios";
import { StateSelect } from "./Components/StateSelect";
import { ColorPicker } from "./Components/ColorPicker";
import { CustomSlider } from "./Components/CustomSlider";

import { BrightnessMedium, Update } from "@material-ui/icons";

const useStyles = makeStyles((theme: Theme) => ({
  root: {
    width: "100%",
    position: "fixed",
  },
}));

interface CurrentValues {
  mode: string;
  color: string;
  brightness: number;
  rate: number;
}

export const App: FunctionComponent = () => {
  const classes = useStyles();
  const [mode, setMode] = React.useState("off");
  const [color, setColor] = React.useState("#ffffff");
  const [brightness, setBrightness] = React.useState(255);
  const [rate, setRate] = React.useState(1.0);

  useEffect(() => {
    (async () => {
      let response = await axios.get("/get?currentvalues").catch((e) => {
        console.log("Error: Could not fetch current values");
      });

      if (typeof response === "undefined") {
        return;
      }

      const data: CurrentValues = response.data;

      setMode(data.mode);
      setColor(data.color);
      setBrightness(data.brightness);
      setRate(data.rate);
    })();
  }, []);

  return (
    <div className={classes.root}>
      <Grid container spacing={2} justify="center" style={{ padding: 16 }}>
        <Grid item>
          <StateSelect buttonMode="off" mode={mode} setMode={setMode} />
        </Grid>
        <Grid item>
          <StateSelect buttonMode="static" mode={mode} setMode={setMode} />
        </Grid>
        <Grid item>
          <StateSelect buttonMode="cycle" mode={mode} setMode={setMode} />
        </Grid>
      </Grid>
      {mode !== "off" ? (
        <Grid container spacing={2} justify="center" style={{ padding: 16 }}>
          <Grid item>
            <BrightnessMedium style={{ color: "#ffffff" }} />
          </Grid>
          <Grid item xs sm={7} md={5} lg={3} xl={3}>
            <CustomSlider
              value={brightness}
              setValue={setBrightness}
              range={[0, 255]}
              step={1}
              type={"brightness"}
            />
          </Grid>
        </Grid>
      ) : (
        <> </>
      )}
      {mode === "static" ? (
        <Grid container spacing={2} justify="center" style={{ padding: 16 }}>
          <ColorPicker color={color} setColor={setColor} />
        </Grid>
      ) : (
        <> </>
      )}
      {mode === "cycle" ? (
        <Grid container spacing={2} justify="center" style={{ padding: 16 }}>
          <Grid item>
            <Update style={{ color: "#ffffff" }} />
          </Grid>
          <Grid item xs sm={7} md={5} lg={3} xl={3}>
            <CustomSlider
              value={rate}
              setValue={setRate}
              range={[0, 5]}
              step={0.01}
              type={"rate"}
            />
          </Grid>
        </Grid>
      ) : (
        <> </>
      )}
    </div>
  );
};
