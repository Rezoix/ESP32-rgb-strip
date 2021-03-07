import React, { createRef, FunctionComponent, MutableRefObject, RefObject, useEffect, useRef } from "react";
import iro from "@jaames/iro";
import { IroColorPicker } from "@jaames/iro/dist/ColorPicker";
import { throttle } from "../util";
import axios from "axios";

interface ColorPickerProps {
  color: string;
  setColor: React.Dispatch<React.SetStateAction<string>>;
}

export const ColorPicker: FunctionComponent<ColorPickerProps> = (props: ColorPickerProps) => {
  let colorPicker: MutableRefObject<IroColorPicker | null> = useRef<IroColorPicker | null>(null);
  let el: RefObject<HTMLDivElement> = createRef<HTMLDivElement>();

  const onChange = (color: string) => {
    const url = "/set?color=" + color.slice(1);

    axios.post(url).catch((e) => {
      console.log("Error: Failed to change mode");
      //console.log(e);
      return;
    });

    props.setColor(color);
  };

  const throttledOnChange = throttle((color: string) => onChange(color));

  useEffect(() => {
    if (!el.current) {
      return;
    }
    if (!colorPicker.current) {
      colorPicker.current = iro.ColorPicker(el.current, {
        color: props.color,
        layout: [{ component: iro.ui.Wheel }],
        width: 400,
        handleRadius: 12,
      });

      if (!colorPicker.current) {
        return;
      }

      colorPicker.current.on("color:change", (color: { hexString: string }) => {
        throttledOnChange(color.hexString);
      });
    } else if (props.color !== colorPicker.current.color.hexString) {
      colorPicker.current.color.set(props.color);
    }
  });

  return <div ref={el} />;
};
