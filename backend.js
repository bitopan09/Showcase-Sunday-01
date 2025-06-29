const express = require("express");
const cors = require("cors"); 
const app = express();
app.use(cors());
app.use(express.json());
const GEMINI_API_KEY = "yourpasskey"; 
const GEMINI_ENDPOINT = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent";
const GEMINI_MODEL = "gemini-1.5-flash"; 
app.post("/gemini", async (req, res) => {
  const userPrompt = req.body.prompt;
  if (!userPrompt) {
    console.log("Error: No prompt provided in the request body from ESP32.");
    return res.status(400).send("Error: Prompt required in request body.");
  }

  console.log(`Received prompt from ESP32: "${userPrompt}"`);

  try {
    const geminiRequestBody = {
      contents: [
        {
          role: "user",
          parts: [
            { text: userPrompt }
          ]
        }
      ],
    };
    const response = await fetch(`${GEMINI_ENDPOINT}?key=${GEMINI_API_KEY}`, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(geminiRequestBody),
    });
    if (!response.ok) {
      const errorText = await response.text(); 
      console.error(
        `Gemini API returned an error! Status: ${response.status}, Body: ${errorText}`
      );
      return res
        .status(response.status)
        .send(`Gemini API Error (${response.status}): ${errorText}`);
    }
    const data = await response.json();
    console.log("Raw Gemini response data:", JSON.stringify(data, null, 2));
    const reply = data?.candidates?.[0]?.content?.parts?.[0]?.text;

    if (reply) {
      console.log(`Sending reply to ESP32: "${reply.trim()}"`);
      res.send(reply.trim());
    } else {
      console.error("Gemini response did not contain expected text content structure.");
      res.status(500).send("Error: Gemini response missing content.");
    }
  } catch (error) {
    console.error("Backend server error during Gemini API call:", error);
    res.status(500).send(`Backend server error: ${error.message}`);
  }
});
const PORT = process.env.PORT || 3000;
app.listen(PORT, () => console.log(`Gemini proxy backend running on port ${PORT}`));
